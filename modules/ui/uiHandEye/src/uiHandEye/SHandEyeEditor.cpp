/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "uiHandEye/SHandEyeEditor.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>

#include <data/BufferTL.hpp>
#include <data/TransformationMatrix3D.hpp>
#include <data/Vector.hpp>

#include <services/macros.hpp>

#include <QDialog>
#include <QHBoxLayout>

#include <ui/qt/container/QtContainer.hpp>

#include <map>

namespace uiHandEye
{
fwServicesRegisterMacro(ui::base::editor::IEditor, ::uiHandEye::SHandEyeEditor)

const core::com::Slots::SlotKeyType SHandEyeEditor::s_ADD_SLOT = "add";
const core::com::Slots::SlotKeyType SHandEyeEditor::s_REMOVE_SLOT        = "remove";
const core::com::Slots::SlotKeyType SHandEyeEditor::s_RESET_SLOT         = "reset";
const core::com::Slots::SlotKeyType SHandEyeEditor::s_GET_SELECTION_SLOT = "getSelection";

static const services::IService::KeyType s_MATRIXVECTOR1_INOUT = "matrixVector1";
static const services::IService::KeyType s_MATRIXVECTOR2_INOUT = "matrixVector2";

static const services::IService::KeyType s_MATRIXTL1_INPUT = "matrixTL1";
static const services::IService::KeyType s_MATRIXTL2_INPUT = "matrixTL2";

// ----------------------------------------------------------------------------

SHandEyeEditor::SHandEyeEditor() noexcept
{
    FW_DEPRECATED_MSG("This service is not supported anymore, it will be removed in sight 21.0.", "21.0");

    newSlot( s_ADD_SLOT,  &SHandEyeEditor::add, this );
    newSlot( s_REMOVE_SLOT, &SHandEyeEditor::remove, this );
    newSlot( s_RESET_SLOT,  &SHandEyeEditor::reset, this );
    newSlot( s_GET_SELECTION_SLOT, &SHandEyeEditor::getSelection, this );

    core::com::HasSlots::m_slots.setWorker( m_associatedWorker );
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::updating()
{
    data::Vector::sptr matrixVector1 = this->getInOut< data::Vector>(s_MATRIXVECTOR1_INOUT);
    data::Vector::sptr matrixVector2 = this->getInOut< data::Vector>(s_MATRIXVECTOR2_INOUT);

    m_capturesListWidget->clear();

    const size_t size1 = matrixVector1->getContainer().size();
    const size_t size2 = matrixVector1->getContainer().size();

    SLM_ASSERT("The two data::Vector have not the same size", size1 == size2);

    for(size_t i = 0; i < size1; ++i )
    {
        const QString countString = QString("%1. Matrix 1 and Matrix 2").arg(i);
        m_capturesListWidget->addItem(countString);
    }
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::configuring()
{
    ui::base::IGuiContainerSrv::initialize();
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::starting()
{
    ui::base::IGuiContainerSrv::create();
    ui::qt::container::QtContainer::sptr qtContainer =
        ui::qt::container::QtContainer::dynamicCast(getContainer());

    // Creation of the Qt elements

    // Main container, VBox
    QVBoxLayout* vLayout = new QVBoxLayout();

    //   First HBox, displays number of items and the remove button
    QHBoxLayout* nbItemsHBox = new QHBoxLayout();

    //     Fill the nbItemsHBox
    QLabel* label = new QLabel("nb captures:");
    nbItemsHBox->addWidget(label);

    m_nbCapturesLabel = new QLabel("-");
    nbItemsHBox->addWidget(m_nbCapturesLabel);
    nbItemsHBox->addStretch();

    //   The ListWidget
    m_capturesListWidget = new QListWidget();
    QObject::connect(m_capturesListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this,
                     SLOT(onItemDoubleClicked(QListWidgetItem*)));

    // Fill the main VBox
    vLayout->addLayout(nbItemsHBox);
    vLayout->addWidget(m_capturesListWidget);

    qtContainer->setLayout(vLayout);

    this->updating();
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::stopping()
{
    this->destroy();
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::add()
{
    data::Vector::sptr matrixVector1 = this->getInOut< data::Vector>(s_MATRIXVECTOR1_INOUT);
    data::Vector::sptr matrixVector2 = this->getInOut< data::Vector>(s_MATRIXVECTOR2_INOUT);

    if(this->isStarted())
    {

        // Get the timelines
        data::MatrixTL::csptr matrixTL1 = this->getInput< data::MatrixTL>(s_MATRIXTL1_INPUT);
        data::MatrixTL::csptr matrixTL2 = this->getInput< data::MatrixTL>(s_MATRIXTL2_INPUT);

        const core::HiResClock::HiResClockType timestamp = core::HiResClock::getTimeInMilliSec();

        if (timestamp > m_lastTimestamp)
        {
            const CSPTR(data::MatrixTL::BufferType) buffer1 = matrixTL1->getClosestBuffer(timestamp);

            const CSPTR(data::MatrixTL::BufferType) buffer2 = matrixTL2->getClosestBuffer(timestamp);

            m_lastTimestamp = timestamp;

            if(buffer1 && buffer2)
            {

                const float* matBuffer1 = buffer1->getElement(0);
                const float* matBuffer2 = buffer2->getElement(0);

                data::TransformationMatrix3D::sptr matrix1 = data::TransformationMatrix3D::New();
                data::TransformationMatrix3D::sptr matrix2 = data::TransformationMatrix3D::New();

                for(unsigned int i = 0; i < 4; ++i)
                {
                    for(unsigned int j = 0; j < 4; ++j)
                    {
                        matrix1->setCoefficient(i, j, static_cast<double>(matBuffer1[4*i+j]));
                        matrix2->setCoefficient(i, j, static_cast<double>(matBuffer2[4*i+j]));
                    }
                }

                matrixVector1->getContainer().push_back(matrix1);
                matrixVector2->getContainer().push_back(matrix2);

                // send data modified signals
                {
                    auto sig1 = matrixVector1->signal< data::Vector::ModifiedSignalType >(
                        data::Vector::s_MODIFIED_SIG);
                    core::com::Connection::Blocker block(sig1->getConnection(m_slotUpdate));
                    sig1->asyncEmit();
                }

                {
                    auto sig2 = matrixVector2->signal< data::Vector::ModifiedSignalType >(
                        data::Vector::s_MODIFIED_SIG);
                    core::com::Connection::Blocker block(sig2->getConnection(m_slotUpdate));
                    sig2->asyncEmit();
                }
            }
        }
    }

    this->updating();
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::remove()
{
    const int idx = m_capturesListWidget->currentRow();

    if(idx >= 0)
    {
        data::Vector::sptr matrixVector1 = this->getInOut< data::Vector>(s_MATRIXVECTOR1_INOUT);
        data::Vector::sptr matrixVector2 = this->getInOut< data::Vector>(s_MATRIXVECTOR2_INOUT);

        data::Vector::IteratorType it1 = matrixVector1->begin();
        data::Vector::IteratorType it2 = matrixVector2->begin();

        std::advance(it1, idx);
        std::advance(it2, idx);

        if(it1 != matrixVector1->end() &&
           it2 != matrixVector2->end())
        {
            matrixVector1->getContainer().erase(it1);
            matrixVector2->getContainer().erase(it2);

            // send data modified signals

            {
                auto sig1 = matrixVector1->signal< data::Vector::ModifiedSignalType >(
                    data::Vector::s_MODIFIED_SIG);
                core::com::Connection::Blocker block(sig1->getConnection(m_slotUpdate));
                sig1->asyncEmit();
            }

            {
                auto sig2 = matrixVector2->signal< data::Vector::ModifiedSignalType >(
                    data::Vector::s_MODIFIED_SIG);
                core::com::Connection::Blocker block(sig2->getConnection(m_slotUpdate));
                sig2->asyncEmit();
            }
        }

        this->updating();
    }
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::reset()
{
    data::Vector::sptr matrixVector1 = this->getInOut< data::Vector>(s_MATRIXVECTOR1_INOUT);
    data::Vector::sptr matrixVector2 = this->getInOut< data::Vector>(s_MATRIXVECTOR2_INOUT);

    matrixVector1->getContainer().clear();
    matrixVector2->getContainer().clear();

    // send data modified signals
    {
        auto sig1 = matrixVector1->signal< data::Vector::ModifiedSignalType >( data::Vector::s_MODIFIED_SIG);
        core::com::Connection::Blocker block(sig1->getConnection(m_slotUpdate));
        sig1->asyncEmit();
    }

    {
        auto sig2 = matrixVector2->signal< data::Vector::ModifiedSignalType >( data::Vector::s_MODIFIED_SIG);
        core::com::Connection::Blocker block(sig2->getConnection(m_slotUpdate));
        sig2->asyncEmit();
    }

    m_capturesListWidget->clear();
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::getSelection()
{
    const int idx = m_capturesListWidget->currentRow();

    if(idx >= 0)
    {
        data::Vector::sptr matrixVector1 = this->getInOut< data::Vector>(s_MATRIXVECTOR1_INOUT);
        data::Vector::sptr matrixVector2 = this->getInOut< data::Vector>(s_MATRIXVECTOR2_INOUT);

        data::Vector::IteratorType it1 = matrixVector1->begin();
        data::Vector::IteratorType it2 = matrixVector2->begin();
        std::advance(it1, idx);
        std::advance(it2, idx);

        auto matrix1 = data::TransformationMatrix3D::dynamicCast(*it1);

        SLM_ASSERT("This element of the vector is not a TransformationMatrix3D", matrix1);

        auto matrix2 = data::TransformationMatrix3D::dynamicCast(*it2);

        SLM_ASSERT("This element of the vector is not a TransformationMatrix3D", matrix2);

        // TODO : Launch a dialog with a view of the two corresponding matrices
        QPointer<QDialog> dialog     = new QDialog();
        QPointer<QGridLayout> layout = new QGridLayout();

        QBoxLayout* m1Layout = new QBoxLayout(QBoxLayout::TopToBottom);
        m1Layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        QBoxLayout* m2Layout = new QBoxLayout(QBoxLayout::TopToBottom);
        m2Layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        QGridLayout* gridLayout1 = new QGridLayout();
        QGridLayout* gridLayout2 = new QGridLayout();
        QVector< QPointer< QLabel > > matrix1Labels, matrix2Labels;

        for(int i = 0; i < 4; ++i)
        {
            for(int j = 0; j < 4; ++j)
            {
                QLabel* label1 = new QLabel("");
                QLabel* label2 = new QLabel("");
                // matrix1
                matrix1Labels.push_back(label1);
                matrix1Labels[i*4 + j]->setText(QString("%1").arg(matrix1->getCoefficient(i, j)));
                gridLayout1->addWidget(label1, i, j);
                // matrix2
                matrix2Labels.push_back(label2);
                matrix2Labels[i*4 + j]->setText(QString("%1").arg(matrix2->getCoefficient(i, j)));
                gridLayout2->addWidget(label2, i, j);

            }
        }
        m1Layout->addLayout(gridLayout1);
        m2Layout->addLayout(gridLayout2);

        QPointer<QPushButton> okButton = new QPushButton("OK");
        okButton->setDefault(true);

        QPointer<QHBoxLayout> buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(okButton);
        QLabel* m1 = new QLabel("matrix 1 :");
        QLabel* m2 = new QLabel("matrix 2 :");

        layout->addWidget(m1, 0, 0);
        layout->addWidget(m2, 0, 2);

        layout->addLayout(m1Layout, 1, 0);
        layout->addLayout(m2Layout, 1, 2);

        layout->addLayout(buttonLayout, 2, 1, 2, 2 );

        QObject::connect(okButton.data(), &QPushButton::clicked, dialog.data(), &QDialog::accept);

        dialog->setLayout(layout);

        dialog->exec();

    }
}

// ----------------------------------------------------------------------------

void SHandEyeEditor::onItemDoubleClicked(QListWidgetItem*)
{
    this->getSelection();
}

// ----------------------------------------------------------------------------

}

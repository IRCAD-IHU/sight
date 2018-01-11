/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuQt/BasicFloatEditor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/Float.hpp>
#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <QApplication>
#include <QDoubleValidator>
#include <QHBoxLayout>

namespace uiVisuQt
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisuQt::BasicFloatEditor );

BasicFloatEditor::BasicFloatEditor() noexcept
{
}

//------------------------------------------------------------------------------

BasicFloatEditor::~BasicFloatEditor() noexcept
{
}

//------------------------------------------------------------------------------

void BasicFloatEditor::starting()
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    ::fwData::Float::sptr floatObj = this->getInOut< ::fwData::Float >("float");

    QHBoxLayout* layout               = new QHBoxLayout();
    QDoubleValidator* doubleValidator = new QDoubleValidator( m_valueCtrl );

    m_valueCtrl = new QLineEdit(  );
    m_valueCtrl->setValidator( doubleValidator );
    layout->addWidget( m_valueCtrl, 1);

    qtContainer->setLayout( layout );

    QObject::connect(m_valueCtrl, SIGNAL(textChanged(QString)), this, SLOT(onModifyValue(QString)));

    this->updating();
}

//------------------------------------------------------------------------------

void BasicFloatEditor::stopping()
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_valueCtrl, SIGNAL(textChanged(QString)), this, SLOT(onModifyValue(QString)));

    this->destroy();
}

//------------------------------------------------------------------------------

void BasicFloatEditor::configuring()
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void BasicFloatEditor::updating()
{
    ::fwData::Float::sptr floatObj = this->getInOut< ::fwData::Float >("float");
    SLM_ASSERT("The given float object is null", floatObj);

    std::stringstream ss;
    ss << floatObj->value();
    QString newText = QString::fromStdString(ss.str());
    if(newText != m_valueCtrl->text())
    {
        m_valueCtrl->setText(newText);
    }
    OSLM_TRACE(floatObj->getID() << " updated value : " << ss.str());
}

//------------------------------------------------------------------------------

void BasicFloatEditor::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------

void BasicFloatEditor::info( std::ostream& _sstream )
{
    _sstream << "Float Editor";
}

//------------------------------------------------------------------------------

void BasicFloatEditor::onModifyValue(QString value)
{
    SLM_TRACE_FUNC();
    ::fwData::Float::sptr floatObj = this->getInOut< ::fwData::Float >("float");
    ::fwData::Float::sptr oldValue;
    oldValue = ::fwData::Object::copy(floatObj);

    std::string strValue = value.toStdString();

    if (strValue.empty() || strValue == "-")
    {
        floatObj->value() = 0;
    }
    else if (value.endsWith( QString(",") ))
    {
        m_valueCtrl->setText( m_valueCtrl->text().replace(QChar(','), QChar('.')) );
    }
    else
    {
        int pos      = 0;
        QString str  = m_valueCtrl->text();
        bool isValid = (m_valueCtrl->validator()->validate(str, pos) == QValidator::Acceptable);

        if (isValid)
        {
            floatObj->value() = ::boost::lexical_cast< ::fwData::Float::ValueType >(strValue);
        }
        else
        {
            this->updating();
        }
    }

    if ( *oldValue != *floatObj )
    {
        OSLM_TRACE(floatObj->getID() << " new value : " << *floatObj);

        auto sig = floatObj->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

//------------------------------------------------------------------------------

} // namespace uiVisuQt

/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "uiTools/editor/SShowVectorSize.hpp"

#include <core/base.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/operations.hpp>

#include <services/IService.hpp>
#include <services/macros.hpp>

#include <QHBoxLayout>
#include <QString>

#include <ui/qt/container/QtContainer.hpp>

#include <filesystem>

namespace uiTools
{

namespace editor
{

const core::com::Slots::SlotKeyType SShowVectorSize::s_ADD_OBJECTS_SLOT    = "addObject";
const core::com::Slots::SlotKeyType SShowVectorSize::s_REMOVE_OBJECTS_SLOT = "removeObjects";

fwServicesRegisterMacro( ::sight::ui::base::editor::IEditor, ::uiTools::editor::SShowVectorSize, ::sight::data::Vector )

//-----------------------------------------------------------------------------

SShowVectorSize::SShowVectorSize() noexcept :
    m_vectorSize(0),
    m_textToShow("")
{
    newSlot(s_ADD_OBJECTS_SLOT, &SShowVectorSize::addObjects, this);
    newSlot(s_REMOVE_OBJECTS_SLOT, &SShowVectorSize::removeObjects, this);

}

//------------------------------------------------------------------------------

SShowVectorSize::~SShowVectorSize() noexcept
{
}

//------------------------------------------------------------------------------

void SShowVectorSize::starting()
{
    this->create();
    ui::qt::container::QtContainer::sptr qtContainer =
        ui::qt::container::QtContainer::dynamicCast( this->getContainer() );

    QHBoxLayout* layout = new QHBoxLayout();

    m_label = new QLabel();
    QString text = m_textToShow + QString::number(this->m_vectorSize);
    m_label->setText(text);

    layout->addWidget(m_label);

    qtContainer->setLayout(layout);
}

//------------------------------------------------------------------------------

void SShowVectorSize::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SShowVectorSize::configuring()
{
    this->initialize();

    services::IService::ConfigType config = this->getConfigTree();
    m_textToShow = QString::fromStdString(config.get< std::string >("text", ""));

}

//------------------------------------------------------------------------------

void SShowVectorSize::updating()
{
}

//------------------------------------------------------------------------------

void SShowVectorSize::swapping()
{

}

//------------------------------------------------------------------------------

void SShowVectorSize::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

void SShowVectorSize::addObjects(data::Vector::ContainerType objects)
{
    m_vectorSize += objects.size();
    QString text = m_textToShow + QString::number(m_vectorSize);
    m_label->setText(text);
}

//------------------------------------------------------------------------------

void SShowVectorSize::removeObjects(data::Vector::ContainerType objects)
{
    m_vectorSize -= objects.size();
    QString text = m_textToShow + QString::number(m_vectorSize);
    m_label->setText(text);
}

//------------------------------------------------------------------------------

::services::IService::KeyConnectionsMap SShowVectorSize::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push("vector", data::Vector::s_ADDED_OBJECTS_SIG, s_ADD_OBJECTS_SLOT );
    connections.push("vector", data::Vector::s_REMOVED_OBJECTS_SIG, s_REMOVE_OBJECTS_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace auroraTracker

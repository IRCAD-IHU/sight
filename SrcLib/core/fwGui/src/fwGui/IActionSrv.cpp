/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGui/IActionSrv.hpp"

#include "fwGui/dialog/IMessageDialog.hpp"
#include "fwGui/dialog/MessageDialog.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

namespace fwGui
{

const ::fwCom::Slots::SlotKeyType IActionSrv::s_SET_IS_ACTIVE_SLOT     = "setIsActive";
const ::fwCom::Slots::SlotKeyType IActionSrv::s_ACTIVATE_SLOT          = "activate";
const ::fwCom::Slots::SlotKeyType IActionSrv::s_DEACTIVATE_SLOT        = "deactivate";
const ::fwCom::Slots::SlotKeyType IActionSrv::s_SET_IS_EXECUTABLE_SLOT = "setIsExecutable";
const ::fwCom::Slots::SlotKeyType IActionSrv::s_SET_EXECUTABLE_SLOT    = "setExecutable";
const ::fwCom::Slots::SlotKeyType IActionSrv::s_SET_INEXECUTABLE_SLOT  = "setInexecutable";
const ::fwCom::Slots::SlotKeyType IActionSrv::s_SET_VISIBLE_SLOT       = "setVisible";
const ::fwCom::Slots::SlotKeyType IActionSrv::s_SHOW_SLOT              = "show";
const ::fwCom::Slots::SlotKeyType IActionSrv::s_HIDE_SLOT              = "hide";

const ::fwCom::Signals::SignalKeyType IActionSrv::s_ENABLED_SIG   = "enabled";
const ::fwCom::Signals::SignalKeyType IActionSrv::s_DISABLED_SIG  = "disabled";
const ::fwCom::Signals::SignalKeyType IActionSrv::s_CHECKED_SIG   = "checked";
const ::fwCom::Signals::SignalKeyType IActionSrv::s_UNCHECKED_SIG = "unchecked";

IActionSrv::IActionSrv()
{
    newSlot(s_SET_IS_ACTIVE_SLOT, &IActionSrv::setIsActive, this);
    newSlot(s_ACTIVATE_SLOT, &IActionSrv::activate, this);
    newSlot(s_DEACTIVATE_SLOT, &IActionSrv::deactivate, this);
    newSlot(s_SET_IS_EXECUTABLE_SLOT, &IActionSrv::setIsExecutable, this);
    newSlot(s_SET_EXECUTABLE_SLOT, &IActionSrv::setExecutable, this);
    newSlot(s_SET_INEXECUTABLE_SLOT, &IActionSrv::setInexecutable, this);
    newSlot(s_SET_VISIBLE_SLOT, &IActionSrv::setVisible, this);
    newSlot(s_SHOW_SLOT, &IActionSrv::show, this);
    newSlot(s_HIDE_SLOT, &IActionSrv::hide, this);

    m_sigEnabled   = newSignal< EnabledSignalType >(s_ENABLED_SIG);
    m_sigDisabled  = newSignal< DisabledSignalType >(s_DISABLED_SIG);
    m_sigChecked   = newSignal< CheckedSignalType >(s_CHECKED_SIG);
    m_sigUnchecked = newSignal< UncheckedSignalType >(s_UNCHECKED_SIG);
}

//-----------------------------------------------------------------------------

IActionSrv::~IActionSrv()
{
}

//-----------------------------------------------------------------------------

void IActionSrv::initialize()
{
    m_registrar = ::fwGui::registrar::ActionRegistrar::New(this->getID());

    OSLM_ASSERT("Depreciated tag <name> in "<< this->getID() << " configuration.", !m_configuration->hasAttribute(
                    "name"));
    OSLM_ASSERT("Depreciated tag <shortcut> in "<< this->getID() << " configuration.",
                !m_configuration->hasAttribute("shortcut"));
    OSLM_ASSERT("Depreciated tag <enable> in "<< this->getID() << " configuration.",
                !m_configuration->hasAttribute("enable"));
    OSLM_ASSERT(
        "Depreciated tag <specialAction> in "<< this->getID() << " configuration.",
            !m_configuration->hasAttribute("specialAction"));
    OSLM_ASSERT("Depreciated tag <style> in "<< this->getID() << " configuration.",
                !m_configuration->hasAttribute("style"));
    OSLM_ASSERT("Depreciated tag <state> in "<< this->getID() << " configuration.",
                !m_configuration->hasAttribute("state"));

    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for( iter = m_configuration->begin(); iter != m_configuration->end(); ++iter )
    {
        if( (*iter)->getName() == "state" )
        {
            ConfigurationType stateCfg = *iter;

            if( stateCfg->hasAttribute("inverse") )
            {
                std::string invertState = stateCfg->getExistingAttributeValue("inverse");
                SLM_ASSERT("Wrong attribute value : must be 'true' or 'false'",
                           (invertState == "true") || (invertState == "false"));
                m_isInverted = (invertState == "true");
            }

            if( stateCfg->hasAttribute("active") )
            {
                std::string isActive = stateCfg->getExistingAttributeValue("active");
                SLM_ASSERT("Wrong attribute value : must be 'true' or 'false'",
                           (isActive == "true") || (isActive == "false"));
                m_isActive = (isActive == "true");
            }

            if( stateCfg->hasAttribute("executable") )
            {
                std::string isExecutable = stateCfg->getExistingAttributeValue("executable");
                SLM_ASSERT("Wrong attribute value : must be 'true' or 'false'",
                           (isExecutable == "true") || (isExecutable == "false"));
                m_isExecutable = (isExecutable == "true");
            }

            if( stateCfg->hasAttribute("visible") )
            {
                std::string isVisible = stateCfg->getExistingAttributeValue("visible");
                SLM_ASSERT("Wrong attribute value : must be 'true' or 'false'",
                           (isVisible == "true") || (isVisible == "false"));
                m_isVisible = (isVisible == "true");
            }
        }

        if( (*iter)->getName() == "confirmation" )
        {
            ConfigurationType cfg = *iter;

            SLM_ASSERT("Missing attribute 'value'", cfg->hasAttribute("value"));
            std::string confirm = cfg->getExistingAttributeValue("value");
            SLM_ASSERT("Wrong attribute value : must be 'true' or 'false'",
                       (confirm == "true") || (confirm == "false"));
            m_confirmAction = (confirm == "true");

            if( cfg->hasAttribute("message") )
            {
                m_confirmMessage = cfg->getExistingAttributeValue("message");
            }

            if( cfg->hasAttribute("defaultbutton") )
            {
                m_defaultButton = cfg->getExistingAttributeValue("defaultbutton");
            }

        }
    }
}

//-----------------------------------------------------------------------------

void IActionSrv::actionServiceStopping()
{
    this->m_registrar->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void IActionSrv::actionServiceStarting()
{
    this->m_registrar->actionServiceStarting();
    this->setIsActive(m_isActive);
}

//-----------------------------------------------------------------------------

void IActionSrv::setIsActive(bool isActive)
{
    m_isActive = isActive;

    this->m_registrar->actionServiceSetActive(m_isActive);
    if(m_isActive)
    {
        m_sigChecked->asyncEmit();
    }
    else
    {
        m_sigUnchecked->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void IActionSrv::activate()
{
    this->setIsActive(true);
}

//-----------------------------------------------------------------------------

void IActionSrv::deactivate()
{
    this->setIsActive(false);
}

//-----------------------------------------------------------------------------

bool IActionSrv::getIsActive() const
{
    return m_isActive;
}

//-----------------------------------------------------------------------------

void IActionSrv::setIsExecutable(bool isExecutable)
{
    m_isExecutable = isExecutable;

    this->m_registrar->actionServiceSetExecutable(m_isExecutable);
    if(m_isExecutable)
    {
        m_sigEnabled->asyncEmit();
    }
    else
    {
        m_sigDisabled->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void IActionSrv::setExecutable()
{
    this->setIsExecutable(true);
}

//-----------------------------------------------------------------------------

void IActionSrv::setInexecutable()
{
    this->setIsExecutable(false);
}

//-----------------------------------------------------------------------------

bool IActionSrv::getIsExecutable() const
{
    return m_isExecutable;
}

//-----------------------------------------------------------------------------

void IActionSrv::setVisible(bool isVisible)
{
    m_isVisible = isVisible;
    this->m_registrar->actionServiceSetVisible(isVisible);
}

//-----------------------------------------------------------------------------

void IActionSrv::show()
{
    this->setVisible(true);
}

//-----------------------------------------------------------------------------

void IActionSrv::hide()
{
    this->setVisible(false);
}

//-----------------------------------------------------------------------------

bool IActionSrv::isVisible() const
{
    return m_isVisible;
}

//-----------------------------------------------------------------------------

bool IActionSrv::getActiveStateValue() const
{
    FW_DEPRECATED_MSG("`getActiveStateValue` is deprecated, please use `isInverted` instead.", "21.0");
    return this->isInverted();
}

//-----------------------------------------------------------------------------

bool IActionSrv::isInverted() const
{
    return m_isInverted;
}

//-----------------------------------------------------------------------------

bool IActionSrv::confirmAction()
{
    bool actionIsConfirmed = true;

    if (m_confirmAction)
    {
        ::fwGui::dialog::MessageDialog dialog;
        dialog.setTitle("Confirmation");
        std::stringstream ss;
        ss << "Do you really want to execute this action ? ";
        if (!m_confirmMessage.empty())
        {
            ss << std::endl << m_confirmMessage;
        }
        dialog.setMessage( ss.str() );

        if(m_defaultButton == "yes")
        {
            dialog.setDefaultButton( ::fwGui::dialog::IMessageDialog::YES );
        }
        else if(m_defaultButton == "no")
        {
            dialog.setDefaultButton( ::fwGui::dialog::IMessageDialog::NO );
        }
        else if(!m_defaultButton.empty())
        {
            SLM_WARN("unknown button: " + m_defaultButton);
        }

        dialog.setIcon( ::fwGui::dialog::IMessageDialog::QUESTION );
        dialog.addButton( ::fwGui::dialog::IMessageDialog::YES_NO );
        ::fwGui::dialog::IMessageDialog::Buttons button = dialog.show();

        actionIsConfirmed = (button == ::fwGui::dialog::IMessageDialog::YES);
    }

    return actionIsConfirmed;
}
//-----------------------------------------------------------------------------

}

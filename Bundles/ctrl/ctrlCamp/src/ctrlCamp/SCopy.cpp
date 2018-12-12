/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ctrlCamp/SCopy.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataCamp/exception/ObjectNotFound.hpp>
#include <fwDataCamp/getObject.hpp>

namespace ctrlCamp
{

fwServicesRegisterMacro(::ctrlCamp::ICamp, ::ctrlCamp::SCopy, ::fwData::Object);

const ::fwServices::IService::KeyType s_SOURCE_INPUT = "source";
const ::fwServices::IService::KeyType s_TARGET_INOUT = "target";

//-----------------------------------------------------------------------------

SCopy::SCopy() :
    m_mode(ModeType::UPDATE)
{

}

//-----------------------------------------------------------------------------

SCopy::~SCopy()
{
}

//-----------------------------------------------------------------------------

void SCopy::configuring()
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    m_hasExtractTag = false;
    const ConfigurationType inCfg = m_configuration->findConfigurationElement("in");
    SLM_ASSERT("One 'in' tag is required.", inCfg);

    const std::vector< ConfigurationType > inoutCfg = m_configuration->find("inout");
    const std::vector< ConfigurationType > outCfg   = m_configuration->find("out");
    SLM_ASSERT("One 'inout' or one 'out' tag is required.", inoutCfg.size() +  outCfg.size() == 1);

    const std::vector< ConfigurationType > extractCfg = inCfg->find("extract");
    SLM_ASSERT("Only one 'extract' tag is authorized.", extractCfg.size() <= 1);
    if (extractCfg.size() == 1)
    {
        ConfigurationType cfg = extractCfg[0];
        SLM_ASSERT("Missing attribute 'from'.", cfg->hasAttribute("from"));
        m_path          = cfg->getAttributeValue("from");
        m_hasExtractTag = true;
    }

    const ConfigurationType modeConfig = m_configuration->findConfigurationElement("mode");
    if (modeConfig)
    {
        auto mode = modeConfig->getValue();
        if(mode == "copyOnStart")
        {
            m_mode = ModeType::START;
        }
        else if(mode == "copyOnUpdate")
        {
            m_mode = ModeType::UPDATE;
        }
        else
        {
            SLM_ERROR("Mode " + mode + " unknown. It should be either 'copyOnStart' or 'copyOnUpdate'");
        }
    }
}

//-----------------------------------------------------------------------------

void SCopy::starting()
{
    if(m_mode == ModeType::START)
    {
        this->copy();
    }
}

//-----------------------------------------------------------------------------

void SCopy::updating()
{
    if(m_mode == ModeType::UPDATE)
    {
        this->copy();
    }
    else
    {
        SLM_ERROR("Object copy was request but the mode is to 'copyOnStart'");
    }
}

//-----------------------------------------------------------------------------

void SCopy::stopping()
{
    // Unregister output
    this->setOutput(s_TARGET_INOUT, nullptr);
}

//-----------------------------------------------------------------------------

void SCopy::copy()
{
    bool create = false;
    ::fwData::Object::sptr target;
    ::fwData::Object::csptr source;
    target = this->getInOut< ::fwData::Object >(s_TARGET_INOUT);
    if(!target)
    {
        create = true;
    }

    ::fwData::Object::csptr sourceObject = this->getInput< ::fwData::Object >(s_SOURCE_INPUT);
    if (m_hasExtractTag)
    {
        ::fwData::Object::sptr object;
        try
        {
            object = ::fwDataCamp::getObject( sourceObject, m_path, true );
        }
        catch(::fwDataCamp::exception::ObjectNotFound&)
        {
            SLM_WARN("Object from '"+ m_path +"' not found");
        }
        catch(std::exception& e)
        {
            OSLM_FATAL("Unhandled exception: " << e.what());
        }

        SLM_WARN_IF("Object from '"+ m_path +"' not found", !object);
        if(object)
        {
            source = object;
        }
    }
    else
    {
        source = sourceObject;
    }

    if(source)
    {
        if(create)
        {
            target = ::fwData::Object::copy(source);
            this->setOutput(s_TARGET_INOUT, target);
        }
        else
        {
            ::fwData::mt::ObjectWriteLock lock(target);

            // copy the object
            target->deepCopy(source);

            auto sig = target->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace ctrlCamp

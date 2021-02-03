/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ioIGTL/STDataListener.hpp"

#include <arData/MatrixTL.hpp>

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <fwData/Composite.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/macros.hpp>

#include <boost/lexical_cast.hpp>

#include <functional>
#include <string>

fwServicesRegisterMacro(::ioIGTL::INetworkListener, ::ioIGTL::STDataListener, ::arData::MatrixTL)

namespace ioIGTL
{

static const std::string s_TIMELINE_KEY = "timeline";

//-----------------------------------------------------------------------------

STDataListener::STDataListener()
{
}

//-----------------------------------------------------------------------------

STDataListener::~STDataListener()
{
}

//-----------------------------------------------------------------------------

void STDataListener::configuring()
{
    SLM_ASSERT("Configuration not found", m_configuration != NULL);
    if (m_configuration->findConfigurationElement("server"))
    {
        const std::string serverInfo = m_configuration->findConfigurationElement("server")->getValue();
        SLM_INFO("OpenIGTLinkListener::configure server: " + serverInfo);
        const std::string::size_type splitPosition = serverInfo.find(':');
        SLM_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        m_hostnameConfig = serverInfo.substr(0, splitPosition);
        m_portConfig     = serverInfo.substr(splitPosition + 1, serverInfo.size());
    }
    else
    {
        throw core::tools::Failed("Server element not found");
    }

    m_deviceNamesConfig = m_configuration->find("deviceName");

    core::runtime::ConfigurationElement::sptr tdata = m_configuration->findConfigurationElement("TData");

    if(tdata)
    {
        std::vector < core::runtime::ConfigurationElement::sptr > matrices = tdata->find("matrix");

        for(const auto& m : matrices)
        {
            const unsigned long index = ::boost::lexical_cast< unsigned long >(m->getAttributeValue("index"));
            m_matrixNameIndex[m->getAttributeValue("name")] = index;
        }
    }
    else
    {
        SLM_WARN("Element 'TData' is missing, the listener will do nothing !");
    }
}

//-----------------------------------------------------------------------------

void STDataListener::runClient()
{
    ::fwGui::dialog::MessageDialog msgDialog;

    ::fwData::Composite::sptr composite = ::fwData::Composite::New();

    // 1. Connection
    try
    {
        const std::uint16_t port   = ::fwPreferences::getValue<std::uint16_t>(m_portConfig);
        const std::string hostname = ::fwPreferences::getValue(m_hostnameConfig);

        if(!m_deviceNamesConfig.empty())
        {
            for(const auto& dn : m_deviceNamesConfig)
            {
                const std::string dnKey = ::fwPreferences::getValue(dn->getValue());
                m_client.addAuthorizedDevice(dnKey);
            }
            m_client.setFilteringByDeviceName(true);
        }

        m_client.connect(hostname, port);
        m_sigConnected->asyncEmit();
    }
    catch (core::Exception& ex)
    {
        // Only open a dialog if the service is started.
        // connect may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->getStatus() == STARTED)
        {
            msgDialog.show("Connection error", ex.what());
            this->slot(s_STOP_SLOT)->asyncRun();
        }
        else
        {
            // Only report the error on console (this normally happens only if we have requested the disconnection)
            SLM_ERROR(ex.what());
        }
        return;
    }

    // 2. Receive messages
    try
    {
        while (m_client.isConnected())
        {
            std::string deviceName;
            double timestamp = 0;
            ::fwData::Object::sptr receiveObject = m_client.receiveObject(deviceName, timestamp);
            if (receiveObject)
            {
                composite->shallowCopy(receiveObject);
                this->manageTimeline(composite, timestamp);
            }
        }
    }
    catch (core::Exception& ex)
    {
        // Only open a dialog if the service is started.
        // ReceiveObject may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->getStatus() == STARTED)
        {
            msgDialog.show("Error", ex.what());
            this->slot(s_STOP_SLOT)->asyncRun();
        }
        else
        {
            // Only report the error on console (this normally happens only if we have requested the disconnection)
            SLM_ERROR(ex.what());
        }
    }
}

//-----------------------------------------------------------------------------

void STDataListener::starting()
{
    ::arData::MatrixTL::sptr matTL = this->getInOut< ::arData::MatrixTL>(s_TIMELINE_KEY);
    matTL->setMaximumSize(10);
    matTL->initPoolSize(static_cast< unsigned int >(m_matrixNameIndex.size()));

    m_clientFuture = std::async(std::launch::async, std::bind(&STDataListener::runClient, this));
}

//-----------------------------------------------------------------------------

void STDataListener::stopping()
{
    m_client.disconnect();
    m_clientFuture.wait();
    m_sigDisconnected->asyncEmit();
}

//-----------------------------------------------------------------------------

void STDataListener::manageTimeline(const ::fwData::Composite::sptr& obj, double timestamp)
{
    ::arData::MatrixTL::sptr matTL = this->getInOut< ::arData::MatrixTL>(s_TIMELINE_KEY);
    SPTR(::arData::MatrixTL::BufferType) matrixBuf;
    matrixBuf = matTL->createBuffer(timestamp);

    for(const ::fwData::Composite::ContainerType::value_type& elt : obj->getContainer())
    {
        ::fwData::TransformationMatrix3D::csptr transfoMatrix =
            ::fwData::TransformationMatrix3D::dynamicConstCast(elt.second);

        MatrixNameIndexType::const_iterator it = m_matrixNameIndex.find(elt.first);

        if(transfoMatrix && it != m_matrixNameIndex.end())
        {
            unsigned long index = it->second;

            ::fwData::TransformationMatrix3D::TMCoefArray values;
            values = transfoMatrix->getCoefficients();
            float matrixValues[16];
            bool isZero = true;
            for(unsigned int i = 0; i < 16; ++i)
            {
                matrixValues[i] = static_cast< float >(values[i]);
                //Test if matrix contains only '0' except last value (always '1)
                isZero &= i < 15 ? (matrixValues[i] == 0.f) : true;
            }
            //don't push the matrix if it contains only '0'
            if(!isZero)
            {
                matrixBuf->setElement(matrixValues, static_cast< unsigned int>(index));
            }
        }
    }
    matTL->pushObject(matrixBuf);

    ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = matTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(::arData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "appQml/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/AppConfigParameters.hpp>

#include <fwGuiQt/QtQmlEngine.hpp>

namespace appQml
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::appQml::Plugin");

//------------------------------------------------------------------------------

Plugin::Plugin() noexcept :
    m_configurationName(""),
    m_parametersName("")
{
    std::cout << "/!\\ Experimental QML app is currently used" << std::endl;
}

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    SLM_FATAL_IF("Bundle appQml, missing param config in profile", !this->getBundle()->hasParameter("config"));
    m_configurationName = this->getBundle()->getParameterValue("config");
    if( this->getBundle()->hasParameter("parameters") )
    {
        m_parametersName = this->getBundle()->getParameterValue("parameters");
    }
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SLM_ASSERT("The OSR is already initialized.", !m_appConfigMng );
    SLM_ASSERT("The configuration name parameter is not initialized.", !m_configurationName.empty());

    m_qmlEngine = std::shared_ptr<::fwServices::IQmlEngine>(new ::fwGuiQt::QtQmlEngine());
    m_appConfigMng = ::fwServices::QmlAppConfigManager::New();

    m_appConfigMng->setEngine(m_qmlEngine);
    if( m_parametersName.empty() )
    {
        const ::fwServices::registry::FieldAdaptorType fields;
        m_appConfigMng->setConfig( m_configurationName, fields );
    }
    else
    {
        const ::fwServices::registry::FieldAdaptorType& fields =
            ::fwServices::registry::AppConfigParameters::getDefault()->getParameters( m_parametersName );
        m_appConfigMng->setConfig( m_configurationName, fields );
    }

    m_appConfigMng->launch();
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::uninitialize() noexcept
{
    SLM_ASSERT("The OSR is not initialized.", m_appConfigMng );
    m_appConfigMng->stopAndDestroy();
    m_appConfigMng.reset();
}

//------------------------------------------------------------------------------

} // namespace appQml

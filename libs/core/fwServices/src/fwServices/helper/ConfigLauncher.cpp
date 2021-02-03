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

#include "fwServices/helper/ConfigLauncher.hpp"

#include <fwServices/macros.hpp>

#include <core/tools/fwID.hpp>

#include <data/Composite.hpp>
#include <data/reflection/getObject.hpp>
#include <data/String.hpp>

#include <boost/property_tree/xml_parser.hpp>

namespace fwServices
{
namespace helper
{

//------------------------------------------------------------------------------

const std::string ConfigLauncher::s_SELF_KEY        = "self";
const std::string ConfigLauncher::s_GENERIC_UID_KEY = "GENERIC_UID";

//------------------------------------------------------------------------------

ConfigLauncher::ConfigLauncher() :
    m_configIsRunning(false)
{
}

//------------------------------------------------------------------------------

ConfigLauncher::~ConfigLauncher()
{
}

//------------------------------------------------------------------------------

void ConfigLauncher::parseConfig(const ::fwServices::IService::ConfigType& _config,
                                 const ::fwServices::IService::sptr& _service)
{
    ::fwServices::IService::ConfigType srvCfg;
    const ::fwServices::IService::ConfigType* curConfig = &_config;

    const ::fwServices::IService::ConfigType& oldConfig = _config;
    SLM_ASSERT("There must be only one <appConfig/> element.", oldConfig.count("appConfig") == 1 );

    const ::fwServices::IService::ConfigType& appConfig = oldConfig.get_child("appConfig");
    const std::string appCfgId                          = appConfig.get<std::string>("<xmlattr>.id");

    srvCfg.add("config.appConfig.<xmlattr>.id", appCfgId);
    ::fwServices::IService::ConfigType& newCfg = srvCfg.get_child("config.appConfig");
    curConfig                                  = &srvCfg;

    auto inoutsCfg = oldConfig.equal_range("inout");
    for (auto itCfg = inoutsCfg.first; itCfg != inoutsCfg.second; ++itCfg)
    {
        ::fwServices::IService::ConfigType parameterCfg;

        const std::string key = itCfg->second.get<std::string>("<xmlattr>.key");
        SLM_ASSERT("[" + appCfgId + "] Missing 'key' tag.", !key.empty());

        const std::string uid = itCfg->second.get<std::string>("<xmlattr>.uid");
        SLM_ASSERT("[" + appCfgId + "] Missing 'uid' tag.", !uid.empty());

        parameterCfg.add("<xmlattr>.replace", key);

        const std::string strOptional = itCfg->second.get<std::string>("<xmlattr>.optional", "no");
        const bool optional           = strOptional == "yes" ? true : false;

        auto obj = _service->getInOut< data::Object>(key);
        if(optional)
        {
            m_optionalInputs[key] = uid;
            parameterCfg.add("<xmlattr>.uid", uid);
        }
        else
        {
            SLM_ASSERT("Object key '" + key + "'with uid '" + uid + "' does not exist.", obj);
            parameterCfg.add("<xmlattr>.uid", obj->getID());
        }

        newCfg.add_child("parameters.parameter", parameterCfg);
    }

    // @deprecated This is no longer necessary to use "uid" to get the prefix replacement, since
    // this is now done in AppConfig. However we keep that code for a while for backward compatibility
    auto paramsCfg = oldConfig.equal_range("parameter");
    for (auto itCfg = paramsCfg.first; itCfg != paramsCfg.second; ++itCfg)
    {
        ::fwServices::IService::ConfigType parameterCfg;

        const std::string replace = itCfg->second.get<std::string>("<xmlattr>.replace");
        SLM_ASSERT("[" + appCfgId + "] Missing 'replace' tag.", !replace.empty());

        parameterCfg.add("<xmlattr>.replace", replace);

        if(itCfg->second.get_child("<xmlattr>").count("uid") == 1)
        {
            FW_DEPRECATED_MSG("'uid' is deprecated for parameters of ConfigLauncher, use 'by' instead", "22.0");
            const std::string uid = itCfg->second.get<std::string>("<xmlattr>.uid");
            parameterCfg.add("<xmlattr>.uid", uid);
        }
        else
        {
            const std::string by = itCfg->second.get<std::string>("<xmlattr>.by");
            parameterCfg.add("<xmlattr>.by", by);
        }

        newCfg.add_child("parameters.parameter", parameterCfg);
    }

    SLM_ASSERT("There must be only one <config/> element.", curConfig->count("config") == 1 );

    const ::fwServices::IService::ConfigType& srvconfig = curConfig->get_child("config");

    SLM_ASSERT("There must be only one <appConfig/> element.", srvconfig.count("appConfig") == 1 );

    const ::fwServices::IService::ConfigType& appConfigCfg = srvconfig.get_child("appConfig");
    m_appConfig = ::fwActivities::registry::ActivityAppConfig(appConfigCfg);
}

//------------------------------------------------------------------------------

void ConfigLauncher::startConfig(::fwServices::IService::sptr _srv,
                                 const FieldAdaptorType& _optReplaceMap )
{
    typedef ::fwActivities::registry::ActivityAppConfig AppConfig;
    FieldAdaptorType replaceMap(_optReplaceMap);

    // Generate generic UID
    const std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier( _srv->getID() );
    replaceMap[ConfigLauncher::s_GENERIC_UID_KEY] = genericUidAdaptor;

    for(const AppConfig::ActivityAppConfigParamsType::value_type& param :  m_appConfig.parameters)
    {
        replaceMap[param.replace] = param.by;
    }

    // Init manager
    m_appConfigManager = ::fwServices::AppConfigManager::New();
    m_appConfigManager->setConfig( m_appConfig.id, replaceMap );

    // When a configuration is launched, deferred objects may already exist.
    // This loop allow to notify the app config manager that this data exist and can be used by services.
    // Whitout that, the data is considered as null.
    for(const auto& [key, uid] : m_optionalInputs)
    {
        auto obj = _srv->getInOut< data::Object >(key);
        if(obj)
        {
            m_appConfigManager->addExistingDeferredObject(obj, uid);
        }
    }

    // Launch config
    m_appConfigManager->launch();

    m_configIsRunning = true;
}

//------------------------------------------------------------------------------

void ConfigLauncher::stopConfig()
{
    if( m_configIsRunning )
    {
        // Delete manager
        m_appConfigManager->stopAndDestroy();
        m_appConfigManager.reset();
    }
    m_configIsRunning = false;
}

//------------------------------------------------------------------------------

}     // helper
} // fwServices

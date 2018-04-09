/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __APPXML_PLUGIN_HPP__
#define __APPXML_PLUGIN_HPP__

#include "TestQML/config.hpp"

#include <fwRuntime/Plugin.hpp>

#include <fwServices/AppConfigManager.hpp>

namespace appXml
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class TESTQML_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TESTQML_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TESTQML_API ~Plugin() noexcept;

    /// Overrides start method. .
    TESTQML_API void start();

    /// Overrides stop method. Do nothing
    TESTQML_API void stop() noexcept;

    TESTQML_API void initialize();

    TESTQML_API void uninitialize() noexcept;

private:
    std::string m_configurationName;
    std::string m_parametersName;
    ::fwServices::AppConfigManager::sptr m_appConfigMng;
};

} // namespace appXml

#endif // __APPXML_PLUGIN_HPP__

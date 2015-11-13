/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLCAMP_PLUGIN_HPP__
#define __CTRLCAMP_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "ctrlCamp/config.hpp"

namespace ctrlCamp
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct CTRLCAMP_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /// Destructor. Do nothing.
    CTRLCAMP_API ~Plugin() throw();

    /**
     * @brief Overrides start method. Object declarations.
     */
    CTRLCAMP_API void start() throw( ::fwRuntime::RuntimeException );

    /// Overrides stop method. Do nothing
    CTRLCAMP_API void stop() throw();

};

} // namespace ctrlCamp

#endif //__CTRLCAMP_PLUGIN_HPP__

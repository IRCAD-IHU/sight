/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENTQT_PLUGIN_HPP__
#define __UIMEASUREMENTQT_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

namespace uiMeasurementQt
{

/**
 *
 * @date    2009.
 */
struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    void start();

    // Overrides
    void stop() noexcept;

};

} // namespace uiMeasurementQt

#endif //__UIMEASUREMENTQT_PLUGIN_HPP__

/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#pragma once

#include "Tuto08GenericSceneCpp/config.hpp"

#include <fwRuntime/Plugin.hpp>

#include <fwServices/AppManager.hpp>

namespace Tuto08GenericSceneCpp
{

/**
 * @brief   This class is started when the module is loaded.
 */
class TUTO08GENERICSCENECPP_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTO08GENERICSCENECPP_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTO08GENERICSCENECPP_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTO08GENERICSCENECPP_API void start();

    /// Overrides stop method. Do nothing
    TUTO08GENERICSCENECPP_API void stop() noexcept;

    TUTO08GENERICSCENECPP_API void initialize();

    TUTO08GENERICSCENECPP_API void uninitialize() noexcept;

private:

    std::unique_ptr< ::fwServices::AppManager > m_appManager;
};

} // namespace Tuto08GenericSceneCpp

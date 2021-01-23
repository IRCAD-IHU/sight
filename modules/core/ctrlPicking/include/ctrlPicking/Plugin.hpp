/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "ctrlPicking/config.hpp"

#include <core/runtime/Plugin.hpp>

namespace ctrlPicking
{

/// @brief This class is started when the module is loaded.
struct CTRLPICKING_CLASS_API Plugin : public core::runtime::Plugin
{

    CTRLPICKING_API ~Plugin();

    CTRLPICKING_API void start();

    CTRLPICKING_API void stop() noexcept;

};

} // namespace ctrlPicking

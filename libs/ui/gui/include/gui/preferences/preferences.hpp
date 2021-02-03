/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2016 IHU Strasbourg
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

#include "gui/preferences/config.hpp"

#include <string>

namespace sight::gui::preferences
{

GUI_API extern const std::string s_VIDEORENDER_PREF; ///< key of the preference composite for VideoRender
// system
GUI_API extern const std::string s_VIDEO_DIR_PREF; ///< key of the video directory in VideoRender preference
// composite

/// Returns the video dir saved in preferences (returns an empty string if it does not exist)
GUI_API std::string getVideoDir();

} // namespace arUtils

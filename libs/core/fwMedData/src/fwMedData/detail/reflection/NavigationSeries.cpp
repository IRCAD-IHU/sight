/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#define CAMP_COMPILATION

#include "fwMedData/NavigationSeries.hpp"

#include "fwMedData/detail/reflection/mapper.hpp"

#include <fwCamp/UserObject.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(NavigationSeries))
{
    builder
    .tag("object_version", "2")
    .tag("lib_name", "fwMedData")
    .base< ::fwMedData::Series>()
    .property("position", &::fwMedData::NavigationSeries::m_position)
    .property("move", &::fwMedData::NavigationSeries::m_move)
    .property("look_at", &::fwMedData::NavigationSeries::m_lookAt)
    ;
}

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

#define CAMP_COMPILATION

#include "fwData/Graph.hpp"

#include "fwData/reflection/mapper.hpp"

#include <core/reflection/Mapper/ArrayMapper.hpp>
#include <core/reflection/UserObject.hpp>

fwCampImplementDataMacro((fwData)(Graph))
{
    builder.base< ::fwData::Object>()
    .tag("object_version", "1")
    .tag("lib_name", "fwData")
    .property("nodes", &::fwData::Graph::m_nodes);
    //.property("connections", &::fwData::Graph::m_connections);
}

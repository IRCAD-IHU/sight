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

#pragma once

#include "data/config.hpp"
#include "data/Material.hpp"

#include <core/reflection/macros.hpp>

fwCampAutoDeclareEnumMacro((sight)(data)(Material)(ShadingType));
fwCampAutoDeclareEnumMacro((sight)(data)(Material)(RepresentationType));
fwCampAutoDeclareEnumMacro((sight)(data)(Material)(OptionsType));
fwCampAutoDeclareEnumMacro((sight)(data)(Material)(FilteringType));
fwCampAutoDeclareEnumMacro((sight)(data)(Material)(WrappingType));

fwCampDeclareAccessor((sight)(data)(Material), (sight)(data)(Color));
fwCampDeclareAccessor((sight)(data)(Material), (sight)(data)(Image));

/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwStructuralPatch/config.hpp"
#include "fwStructuralPatch/creator/data/Series1.hpp"

#include <fwAtomsPatch/IStructuralCreator.hpp>

namespace fwStructuralPatch
{
namespace creator
{
namespace sight::data
{

/// Structural creator for data::ActivitySeries version 1.
class FWSTRUCTURALPATCH_CLASS_API ActivitySeries1 : public ::fwStructuralPatch::creatordata::Series1
{

public:

    fwCoreClassMacro(ActivitySeries1, ::fwStructuralPatch::creatordata::Series1, new ActivitySeries1)

    /// Constructor
    FWSTRUCTURALPATCH_API ActivitySeries1();

    /// Destructor
    FWSTRUCTURALPATCH_API ~ActivitySeries1();

    /// Copy constructor
    FWSTRUCTURALPATCH_API ActivitySeries1( const ActivitySeries1& cpy );

    /**
     * @brief Creates the specified object.
     */
    FWSTRUCTURALPATCH_API virtual atoms::Object::sptr create() override;

};

} // namespace sight::data
} // namespace creator
} // namespace fwStructuralPatch

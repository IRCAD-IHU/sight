/************************************************************************
 *
 * Copyright (C) 2015-2021 IRCAD France
 * Copyright (C) 2015-2019 IHU Strasbourg
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

#include <fwAtomsPatch/IStructuralPatch.hpp>

namespace fwStructuralPatch
{

namespace sight::data
{

namespace Material
{

/// Structural patch to convert a data::Material from version '3' to version '4'.
class FWSTRUCTURALPATCH_CLASS_API V3ToV4 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassMacro(V3ToV4, ::fwStructuralPatchdata::Material::V3ToV4, new V3ToV4)

    /// Constructor
    FWSTRUCTURALPATCH_API V3ToV4();

    /// Destructor
    FWSTRUCTURALPATCH_API ~V3ToV4();

    /// Copy constructor
    FWSTRUCTURALPATCH_API V3ToV4( const V3ToV4& cpy );

    /**
     * @brief Applies patch
     *
     * Added new map of additional array.
     */
    FWSTRUCTURALPATCH_API virtual void apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace Material

} // namespace sight::data

} // namespace fwStructuralPatch

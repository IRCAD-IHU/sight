/************************************************************************
 *
 * Copyright (C) 2015-2021 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

namespace Mesh
{

/// Structural patch to convert a data::Mesh from version '4' to version '3'.
class FWSTRUCTURALPATCH_CLASS_API V4ToV3 : public ::fwAtomsPatch::IStructuralPatch
{
public:
    fwCoreClassMacro(V4ToV3, ::fwStructuralPatchdata::Mesh::V4ToV3, new V4ToV3)

    /// Constructor
    FWSTRUCTURALPATCH_API V4ToV3();

    /// Destructor
    FWSTRUCTURALPATCH_API ~V4ToV3();

    /// Copy constructor
    FWSTRUCTURALPATCH_API V4ToV3( const V4ToV3& cpy );

    /**
     * @brief Applies patch
     *
     * Added new map of additional array.
     */
    FWSTRUCTURALPATCH_API virtual void apply(
        const atoms::Object::sptr& previous,
        const atoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

};

} // namespace Mesh

} // namespace sight::data

} // namespace fwStructuralPatch

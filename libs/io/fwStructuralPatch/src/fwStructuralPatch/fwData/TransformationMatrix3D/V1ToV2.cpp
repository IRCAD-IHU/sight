/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2015 IHU Strasbourg
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

#include "fwStructuralPatch/data/TransformationMatrix3D/V1ToV2.hpp"

#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/StructuralCreatorDB.hpp>

namespace fwStructuralPatch
{

namespace sight::data
{

namespace TransformationMatrix3D
{

V1ToV2::V1ToV2() :
    ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "data::TransformationMatrix3D";
    m_targetClassname = "data::TransformationMatrix3D";
    m_originVersion   = "1";
    m_targetVersion   = "2";
}

// ----------------------------------------------------------------------------

V1ToV2::~V1ToV2()
{
}

// ----------------------------------------------------------------------------

V1ToV2::V1ToV2( const V1ToV2& cpy ) :
    ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToV2::apply( const ::fwAtoms::Object::sptr& previous,
                    const ::fwAtoms::Object::sptr& current,
                    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);

    ::fwAtoms::Sequence::sptr oldCoefs = previous->getAttribute< ::fwAtoms::Sequence >("coefficient");
    ::fwAtoms::Sequence::sptr newCoefs = ::fwAtoms::Sequence::New();

    for(size_t i = 0; i < 16; ++i)
    {
        newCoefs->push_back((*oldCoefs)[i]);
    }

    helper.removeAttribute("coefficient");
    helper.addAttribute("coefficient", newCoefs);
}

} // namespace TransformationMatrix3D

} // namespace sight::data

} // namespace fwStructuralPatch

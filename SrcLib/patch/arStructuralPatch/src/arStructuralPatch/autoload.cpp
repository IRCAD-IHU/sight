/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arStructuralPatch/arData/Camera/V1ToV2.hpp"

#include <fwAtomsPatch/StructuralPatchDB.hpp>



namespace arStructuralPatch
{

/// Registers structural patches dedicated to data conversions.
struct runner
{
    runner()
    {
        ::fwAtomsPatch::StructuralPatchDB::sptr structuralPatches = ::fwAtomsPatch::StructuralPatchDB::getDefault();
        structuralPatches->registerPatch(::arStructuralPatch::arData::Camera::V1ToV2::New());
    }

    static runner r;
};
runner runner::r;

} // namespace arStructuralPatch

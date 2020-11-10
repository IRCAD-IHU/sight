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

#include "fwMDSemanticPatch/PatchLoader.hpp"

#include <fwAtomsPatch/VersionsManager.hpp>

#include <fwRuntime/operations.hpp>

namespace fwMDSemanticPatch
{

std::string PatchLoader::s_currentVersion = "V15";

//------------------------------------------------------------------------------

void PatchLoader::loadPatches()
{
    SPTR(::fwAtomsPatch::VersionsManager) versionManager = ::fwAtomsPatch::VersionsManager::getDefault();
    auto path = ::fwRuntime::getLibraryResourceFilePath("fwMDSemanticPatch-" FWMDSEMANTICPATCH_VER "/");
    versionManager->buildVersionTable(path.string());
    versionManager->buildLinkTable(path.string());
}

//------------------------------------------------------------------------------

void PatchLoader::setCurrentVersion(const std::string& version)
{
    s_currentVersion = version;
}

//------------------------------------------------------------------------------

std::string PatchLoader::getCurrentVersion()
{
    return s_currentVersion;
}

//------------------------------------------------------------------------------

} //namespace fwMDSemanticPatch

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

#include "fwAtomsPatch/config.hpp"
#include "fwAtomsPatch/helper/Object.hpp"
#include "fwAtomsPatch/IPatch.hpp"

#include <core/base.hpp>

#include <string>

namespace sight::atoms
{
class Object;
}

namespace fwAtomsPatch
{

/**
 * @brief Base class of all structural patches
 */
class FWATOMSPATCH_CLASS_API IStructuralPatch : public IPatch
{

public:
    fwCoreClassMacro(IStructuralPatch)
    fwCoreAllowSharedFromThis();

    ///Constructor
    FWATOMSPATCH_API IStructuralPatch();

    ///Copy constructor
    FWATOMSPATCH_API IStructuralPatch( const IStructuralPatch& cpy );

    ///Destructor
    FWATOMSPATCH_API virtual ~IStructuralPatch();

    /**
     * @brief Applies the patch to the specified object
     *
     * @param previous object from which modifications will be applied
     * @param current object receiving modifications
     * @param newVersions map containing already patched sub objects
     *
     * @pre previous and current objects must have the same classname and version.
     */
    FWATOMSPATCH_API virtual void apply(const SPTR(atoms::Object)& previous,
                                        const SPTR(atoms::Object)& current,
                                        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions) override;

    /// Returns the target classname of the object
    FWATOMSPATCH_API virtual const std::string& getTargetClassname() const;

    /// Returns the target version of the object
    FWATOMSPATCH_API virtual const std::string& getTargetVersion() const;

protected:

    /// Updates the version of the patched object
    FWATOMSPATCH_API void updateVersion(SPTR(atoms::Object) current);

    /// Target type
    std::string m_targetClassname;

    /// Target version
    std::string m_targetVersion;
};

} // namespace fwAtomsPatch

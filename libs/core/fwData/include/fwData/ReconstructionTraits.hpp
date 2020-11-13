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

#pragma once

#include "fwData/factory/new.hpp"
#include "fwData/Node.hpp"
#include "fwData/StructureTraits.hpp"

fwCampAutoDeclareDataMacro((fwData)(ReconstructionTraits));

namespace fwData
{

/**
 * @brief This class defines Reconstruction traits.
 *
 * A ReconstructionTraits contains :
 *  - an identifier
 *  - a structure traits associated to the reconstruction
 *  - a mask operator node
 *  - a mesh operator node
 *
 * @see StructureTraits
 */
class FWDATA_CLASS_API ReconstructionTraits : public ::fwData::Object
{
public:
    fwCoreClassMacro(ReconstructionTraits, ::fwData::Object, ::fwData::factory::New< ReconstructionTraits >)
    fwCampMakeFriendDataMacro((fwData)(ReconstructionTraits));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API ReconstructionTraits(::fwData::Object::Key key);

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~ReconstructionTraits();

    /**
     * @{
     * @brief Get/Set value of the identifier.
     */
    std::string& getIdentifier ();
    const std::string& getIdentifier () const;
    void setIdentifier (const std::string& _identifier);
    /// @}

    /// Set the reconstruction operator mask node
    FWDATA_API void setMaskOpNode(const ::fwData::Node::sptr& maskOpNode );

    /// Get the reconstruction operator mask node. Return a null pointer if the mask node is not defined.
    FWDATA_API ::fwData::Node::sptr getMaskOpNode();

    /// Set the reconstruction operator mesh node.
    FWDATA_API void setMeshOpNode(const ::fwData::Node::sptr& meshOpNode );

    /// Get the reconstruction operator mesh node. Return a null pointer if the mesh node is not defined.
    FWDATA_API ::fwData::Node::sptr getMeshOpNode();

    /// Set the associated structure traits
    FWDATA_API void setStructureTraits(const ::fwData::StructureTraits::sptr& structureTraits );

    /// Get the associated structure traits
    FWDATA_API ::fwData::StructureTraits::sptr getStructureTraits();

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

private:

    /// ROI identifier
    std::string m_identifier;

    /// reconstruction mask operator node
    ::fwData::Node::sptr m_maskOpNode;

    /// reconstruction mesh operator node
    ::fwData::Node::sptr m_meshOpNode;

    /// associated structure traits
    ::fwData::StructureTraits::sptr m_structureTraits;

};

//-----------------------------------------------------------------------------

inline std::string& ReconstructionTraits::getIdentifier ()
{
    return m_identifier;
}

//-----------------------------------------------------------------------------

inline const std::string& ReconstructionTraits::getIdentifier () const
{
    return m_identifier;
}

//-----------------------------------------------------------------------------

inline void ReconstructionTraits::setIdentifier (const std::string& _identifier)
{
    m_identifier = _identifier;
}

//-----------------------------------------------------------------------------

} // namespace fwData

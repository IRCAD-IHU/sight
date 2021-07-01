/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "ReconstructionTraitsDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/ReconstructionTraits.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr ReconstructionTraitsDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& reconstructionTraits = safeCast<sight::data::ReconstructionTraits>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::ReconstructionTraits>(tree, 0, 1);

    // Deserialize attributes
    reconstructionTraits->setIdentifier(readString(tree, "Identifier"));

    // Reconstruction mask operator node
    reconstructionTraits->setMaskOpNode(sight::data::Node::dynamicCast(children.at("MaskOpNode")));

    // Reconstruction mesh operator node
    reconstructionTraits->setMeshOpNode(sight::data::Node::dynamicCast(children.at("MeshOpNode")));

    // Associated structure traits
    reconstructionTraits->setStructureTraits(sight::data::StructureTraits::dynamicCast(children.at("StructureTraits")));

    return reconstructionTraits;
}

} // detail::data

} // namespace sight::io::session

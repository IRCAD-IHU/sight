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

#include "ReconstructionDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/Image.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/Reconstruction.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr ReconstructionDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& reconstruction = safeCast<sight::data::Reconstruction>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::Reconstruction>(tree, 0, 1);

    // Deserialize attributes
    reconstruction->setIsVisible(tree.get<bool>("IsVisible"));
    reconstruction->setOrganName(readString(tree, "OrganName"));
    reconstruction->setStructureType(readString(tree, "StructureType"));

    reconstruction->setMaterial(sight::data::Material::dynamicCast(children.at("Material")));
    reconstruction->setImage(sight::data::Image::dynamicCast(children.at("Image")));
    reconstruction->setMesh(sight::data::Mesh::dynamicCast(children.at("Mesh")));

    reconstruction->setComputedMaskVolume(tree.get<double>("ComputedMaskVolume"));

    return reconstruction;
}

} // detail::data

} // namespace sight::io::session

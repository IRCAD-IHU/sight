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

#include "MaterialDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/Material.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr MaterialDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& material = safeCast<sight::data::Material>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::Material>(tree, 0, 1);

    material->setShadingMode(static_cast<sight::data::Material::ShadingType>(tree.get<int>("ShadingMode")));
    material->setRepresentationMode(
        static_cast<sight::data::Material::RepresentationType>(
            tree.get<int>("RepresentationMode")
        )
    );
    material->setOptionsMode(static_cast<sight::data::Material::OptionsType>(tree.get<int>("OptionsMode")));
    material->setDiffuseTextureFiltering(
        static_cast<sight::data::Material::FilteringType>(
            tree.get<int>("DiffuseTextureFiltering")
        )
    );
    material->setDiffuseTextureWrapping(
        static_cast<sight::data::Material::WrappingType>(
            tree.get<int>("DiffuseTextureWrapping")
        )
    );

    material->setAmbient(sight::data::Color::dynamicCast(children.at("Ambient")));
    material->setDiffuse(sight::data::Color::dynamicCast(children.at("Diffuse")));
    material->setDiffuseTexture(sight::data::Image::dynamicCast(children.at("DiffuseTexture")));

    return material;
}

} // detail::data

} // namespace sight::io::session

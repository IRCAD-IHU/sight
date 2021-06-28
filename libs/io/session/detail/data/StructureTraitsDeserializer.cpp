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

#include "StructureTraitsDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/StructureTraits.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr StructureTraitsDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& structureTraits = safeCast<sight::data::StructureTraits>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::StructureTraits>(tree, 0, 1);

    // Deserialize attributes
    structureTraits->setType(readString(tree, "Type"));
    structureTraits->setClass(static_cast<sight::data::StructureTraits::StructureClass>(tree.get<int>("Class")));
    structureTraits->setNativeExp(readString(tree, "NativeExp"));
    structureTraits->setNativeGeometricExp(readString(tree, "NativeGeometricExp"));
    structureTraits->setAttachmentType(readString(tree, "AttachmentType"));
    structureTraits->setAnatomicRegion(readString(tree, "AnatomicRegion"));
    structureTraits->setPropertyCategory(readString(tree, "PropertyCategory"));
    structureTraits->setPropertyType(readString(tree, "PropertyType"));

    // Categories
    auto& categories = structureTraits->getCategories();

    // Clearing is required in case the object is reused
    categories.clear();

    for(const auto& categoryTree : tree.get_child("Categories"))
    {
        categories.push_back(static_cast<sight::data::StructureTraits::Category>(categoryTree.second.get_value<int>()));
    }

    // Color
    structureTraits->setColor(sight::data::Color::dynamicCast(children.at("Color")));

    return structureTraits;
}

} // detail::data

} // namespace sight::io::session

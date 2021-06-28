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

#include "StructureTraitsSerializer.hpp"

#include <data/StructureTraits.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void StructureTraitsSerializer::serialize(
    const zip::ArchiveWriter::sptr&,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string&
) const
{
    const auto& structureTraits = safeCast<sight::data::StructureTraits>(object);

    // Add a version number. Not mandatory, but could help for future release
    writeVersion<sight::data::StructureTraits>(tree, 1);

    // Serialize attributes
    writeString(tree, "Type", structureTraits->getType());
    tree.put("Class", structureTraits->getClass());
    writeString(tree, "NativeExp", structureTraits->getNativeExp());
    writeString(tree, "NativeGeometricExp", structureTraits->getNativeGeometricExp());
    writeString(tree, "AttachmentType", structureTraits->getAttachmentType());
    writeString(tree, "AnatomicRegion", structureTraits->getAnatomicRegion());
    writeString(tree, "PropertyCategory", structureTraits->getPropertyCategory());
    writeString(tree, "PropertyType", structureTraits->getPropertyType());

    // Categories
    boost::property_tree::ptree categoriesTree;

    for(const auto& category : structureTraits->getCategories())
    {
        categoriesTree.add("Category", category);
    }

    tree.add_child("Categories", categoriesTree);

    // Color
    children["Color"] = structureTraits->getColor();
}

} // detail::data

} // namespace sight::io::session

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

#include "ArraySerializer.hpp"

#include <data/Array.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void ArraySerializer::serialize(
    const zip::ArchiveWriter::sptr& archive,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>&,
    const core::crypto::secure_string& password
) const
{
    const auto& array = IDataSerializer::safeCast<sight::data::Array>(object);

    // Add a version number. Not mandatory, but could help for future release
    IDataSerializer::writeVersion<sight::data::Array>(tree, 1);

    // Size
    boost::property_tree::ptree sizesTree;

    for(const auto& size : array->getSize())
    {
        sizesTree.add("Size", size);
    }

    tree.add_child("Sizes", sizesTree);

    // type, isBufferOwner
    tree.put("Type", array->getType().string());
    tree.put("IsBufferOwner", array->getIsBufferOwner());

    // Create the output file inside the archive
    const auto& ostream = archive->openFile(
        std::filesystem::path(array->getUUID() + "/buffer.raw"),
        password,
        zip::Method::ZSTD,
        zip::Level::DEFAULT
    );

    // Write back to the archive
    const auto& buffer = array->getBufferObject();
    ostream->write(static_cast<const char*>(buffer->getBuffer()), static_cast<std::streamsize>(buffer->getSize()));
}

} // detail::data

} // namespace sight::io::session

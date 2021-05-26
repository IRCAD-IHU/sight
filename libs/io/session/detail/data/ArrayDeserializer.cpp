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

#include "ArrayDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/Array.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr ArrayDeserializer::deserialize(
    const zip::ArchiveReader::sptr& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string& password
) const
{
    // Create or reuse the object
    const auto& array = object ? sight::data::Array::dynamicCast(object) : sight::data::Array::New();

    SIGHT_ASSERT(
        "Object '" << array->getClassname() << "' is not a '" << sight::data::Array::classname() << "'",
        array
    );

    // Check version number. Not mandatory, but could help for future release
    const int version = tree.get<int>("version", 0);
    SIGHT_THROW_IF(
        ArrayDeserializer::classname() << " is not implemented for version '" << version << "'.",
        version > 1
    );

    // Type
    array->setType(tree.get<std::string>("Type"));

    // IsBufferOwner
    array->setIsBufferOwner(tree.get<bool>("IsBufferOwner", false));

    // Sizes
    std::vector<size_t> sizes;

    for(const auto& sizeTree : tree.get_child("Sizes"))
    {
        const auto& size = sizeTree.second.get_value<size_t>();
        sizes.push_back(size);
    }

    array->resize(sizes, true);

    // Buffer
    const auto& bufferObject = array->getBufferObject();
    core::memory::BufferObject::Lock lockerSource(bufferObject);

    // Create the istream from the input file inside the archive
    const auto& uuid    = tree.get<std::string>("uuid");
    const auto& istream = archive->openFile(
        std::filesystem::path(uuid + "/buffer.raw"),
        password
    );

    istream->read(static_cast<char*>(bufferObject->getBuffer()), static_cast<std::streamsize>(bufferObject->getSize()));

    return array;
}

} // detail::data

} // namespace sight::io::session

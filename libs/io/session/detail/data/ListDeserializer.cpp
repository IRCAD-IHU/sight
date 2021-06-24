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

#include "ListDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/List.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr ListDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& list = safeCast<sight::data::List>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::List>(tree, 0, 1);

    // Convert the map into a List
    // We assume the key is the index
    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(sight::data::Object::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        list->getContainer().push_back(it->second);
    }

    return list;
}

} // detail::data

} // namespace sight::io::session

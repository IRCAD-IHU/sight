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

#include "PlaneDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/Plane.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr PlaneDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& plane = safeCast<sight::data::Plane>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::Plane>(tree, 0, 1);

    plane->setIsIntersection(tree.get<bool>("IsIntersection"));

    for(size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(sight::data::Point::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        plane->getPoints()[index] = safeCast<sight::data::Point>(it->second);
    }

    return plane;
}

} // detail::data

} // namespace sight::io::session

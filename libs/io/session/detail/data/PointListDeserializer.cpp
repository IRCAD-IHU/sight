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

#include "PointListDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/Point.hpp>
#include <data/PointList.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr PointListDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& pointList = safeCast<sight::data::PointList>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::PointList>(tree, 0, 1);

    // Convert the map into a vector
    // We assume the key is the index
    std::vector<sight::data::Point::sptr> points;
    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& child = children.at(sight::data::Point::classname() + std::to_string(index));
        const auto& point = sight::data::Point::dynamicCast(child);

        points.push_back(point);
    }

    pointList->setPoints(points);

    return pointList;
}

} // detail::data

} // namespace sight::io::session

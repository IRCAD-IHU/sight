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

#include "PointDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/Point.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr PointDeserializer::deserialize(
    const zip::ArchiveReader::sptr& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string& password
) const
{
    // Create or reuse the object
    const auto& point =
        object ? sight::data::Point::dynamicCast(object) : sight::data::Point::New();

    SIGHT_ASSERT(
        "Object '" << point->getClassname() << "' is not a '" << sight::data::Point::classname() << "'",
        point
    );

    // Check version number. Not mandatory, but could help for future release
    const int version = tree.get<int>("version", 0);
    SIGHT_THROW_IF(
        PointDeserializer::classname() << " is not implemented for version '" << version << "'.",
        version > 1
    );

    const std::array<double, 3> coordinates = {
        tree.get<double>("x"),
        tree.get<double>("y"),
        tree.get<double>("z")
    };

    point->setCoord(coordinates);

    return point;
}

} // detail::data

} // namespace sight::io::session

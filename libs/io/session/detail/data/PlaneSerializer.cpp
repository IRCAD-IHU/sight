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

#include "PlaneSerializer.hpp"

#include <data/Plane.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void PlaneSerializer::serialize(
    const zip::ArchiveWriter::sptr&,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string&
) const
{
    const auto& plane = safeCast<sight::data::Plane>(object);

    // Add a version number. Not mandatory, but could help for future release
    writeVersion<sight::data::Plane>(tree, 1);

    tree.put("IsIntersection", plane->getIsIntersection());

    size_t index = 0;
    for(const auto& point : plane->getPoints())
    {
        children[sight::data::Point::classname() + std::to_string(index++)] = point;
    }
}

} // detail::data

} // namespace sight::io::session

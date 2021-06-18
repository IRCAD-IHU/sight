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

#include "PointListSerializer.hpp"

#include <data/PointList.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void PointListSerializer::serialize(
    const zip::ArchiveWriter::sptr&,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string&
) const
{
    const auto& pointList = IDataSerializer::safeCast<sight::data::PointList>(object);

    // Add a version number. Not mandatory, but could help for future release
    IDataSerializer::writeVersion<sight::data::PointList>(tree, 1);

    // Add points to children list
    int index = 0;
    for(const auto& point : pointList->getPoints())
    {
        children[std::to_string(index++)] = point;
    }
}

} // detail::data

} // namespace sight::io::session

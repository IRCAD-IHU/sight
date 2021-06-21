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

#include "LandmarksDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/Landmarks.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr LandmarksDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& landmarks = safeCast<sight::data::Landmarks>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::Landmarks>(tree, 0, 1);

    // Iterate on groups
    for(const auto& groupTree : tree.get_child("Groups"))
    {
        const auto& colorTree            = groupTree.second.get_child("Color");
        const std::array<float, 4> color = {
            colorTree.get<float>("Red"),
            colorTree.get<float>("Green"),
            colorTree.get<float>("Blue"),
            colorTree.get<float>("Alpha"),
        };

        landmarks->addGroup(
            groupTree.first,
            color,
            groupTree.second.get<float>("Size"),
            groupTree.second.get<int>("Shape") == 0
            ? sight::data::Landmarks::Shape::SPHERE
            : sight::data::Landmarks::Shape::CUBE,
            groupTree.second.get<bool>("Visibility")
        );

        const auto& pointsTree = groupTree.second.get_child("Points");
        for(const auto& pointTree : pointsTree)
        {
            const std::array<double, 3> point = {
                pointTree.second.get<double>("X"),
                pointTree.second.get<double>("Y"),
                pointTree.second.get<double>("Z")
            };

            landmarks->addPoint(groupTree.first, point);
        }
    }

    return landmarks;
}

} // detail::data

} // namespace sight::io::session

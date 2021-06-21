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

#include "LandmarksSerializer.hpp"

#include <data/Landmarks.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void LandmarksSerializer::serialize(
    const zip::ArchiveWriter::sptr&,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string&
) const
{
    const auto& landmarks = safeCast<sight::data::Landmarks>(object);

    // Add a version number. Not mandatory, but could help for future release
    writeVersion<sight::data::Landmarks>(tree, 1);

    boost::property_tree::ptree groupsTree;

    for(const auto& name : landmarks->getGroupNames())
    {
        // Serialize landmark group
        const auto& group = landmarks->getGroup(name);

        boost::property_tree::ptree groupTree;
        groupTree.put("Size", group.m_size);
        groupTree.put("Shape", group.m_shape == sight::data::Landmarks::Shape::SPHERE ? 0 : 1);
        groupTree.put("Visibility", group.m_visibility);

        // Group color
        boost::property_tree::ptree colorTree;
        colorTree.put("Red", group.m_color[0]);
        colorTree.put("Green", group.m_color[1]);
        colorTree.put("Blue", group.m_color[2]);
        colorTree.put("Alpha", group.m_color[3]);
        groupTree.add_child("Color", colorTree);

        // Points
        boost::property_tree::ptree pointsTree;

        for(const auto& point : group.m_points)
        {
            boost::property_tree::ptree pointTree;
            pointTree.put("X", point[0]);
            pointTree.put("Y", point[1]);
            pointTree.put("Z", point[2]);
            pointsTree.add_child("Point", pointTree);
        }

        groupTree.add_child("Points", pointsTree);
        groupsTree.add_child(name, groupTree);
    }

    tree.add_child("Groups", groupsTree);
}

} // detail::data

} // namespace sight::io::session

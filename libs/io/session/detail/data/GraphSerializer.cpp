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

#include "GraphSerializer.hpp"

#include <data/Graph.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void GraphSerializer::serialize(
    const zip::ArchiveWriter::sptr&,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string&
) const
{
    const auto& graph = safeCast<sight::data::Graph>(object);

    // Add a version number. Not mandatory, but could help for future release
    writeVersion<sight::data::Graph>(tree, 1);

    // Serialize node
    size_t index = 0;
    for(const auto& node : graph->getNodes())
    {
        children[sight::data::Node::classname() + std::to_string(index++)] = node;
    }

    // Serialize connections
    index = 0;
    for(const auto& connection : graph->getConnections())
    {
        // Serialize edge
        const std::string edgeIndex = sight::data::Edge::classname() + std::to_string(index++);
        children[edgeIndex] = connection.first;

        // Serialize nodes
        children[edgeIndex + "up"]   = connection.second.first;
        children[edgeIndex + "down"] = connection.second.second;
    }
}

} // detail::data

} // namespace sight::io::session

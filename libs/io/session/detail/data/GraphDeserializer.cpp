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

#include "GraphDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/Graph.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr GraphDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& graph = safeCast<sight::data::Graph>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::Graph>(tree, 0, 1);

    // Deserialize node
    for(size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& nodeIt = children.find(sight::data::Node::classname() + std::to_string(index));

        // If we didn't found a matching node, exit the loop
        if(nodeIt == children.cend())
        {
            break;
        }

        const auto& node = safeCast<sight::data::Node>(nodeIt->second);
        graph->addNode(node);
    }

    // Deserialize connections
    for(size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        // Deserialize edge
        const std::string edgeIndex = sight::data::Edge::classname() + std::to_string(index);
        const auto& edgeIt          = children.find(edgeIndex);
        const auto& upIt            = children.find(edgeIndex + "up");
        const auto& downIt          = children.find(edgeIndex + "down");

        // If we didn't found a matching edge or node, exit the loop
        if(edgeIt == children.cend() && upIt == children.cend() && downIt == children.cend())
        {
            break;
        }

        const auto& edge     = safeCast<sight::data::Edge>(edgeIt->second);
        const auto& upNode   = safeCast<sight::data::Node>(upIt->second);
        const auto& downNode = safeCast<sight::data::Node>(downIt->second);

        graph->addEdge(edge, upNode, downNode);
    }

    return graph;
}

} // detail::data

} // namespace sight::io::session

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

#include "NodeDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/Node.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr NodeDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& node = safeCast<sight::data::Node>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::Node>(tree, 0, 1);

    node->setObject(children.at("Object"));

    for(size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& inputIt  = children.find("Input" + std::to_string(index));
        const auto& outputIt = children.find("Output" + std::to_string(index));

        // If we didn't found a matching input or output, exit the loop
        if(inputIt == children.cend() && outputIt == children.cend())
        {
            break;
        }

        if(inputIt != children.cend())
        {
            const auto& inputPort = safeCast<sight::data::Port>(inputIt->second);
            node->addInputPort(inputPort);
        }

        if(outputIt != children.cend())
        {
            const auto& outputPort = safeCast<sight::data::Port>(outputIt->second);
            node->addOutputPort(outputPort);
        }
    }

    return node;
}

} // detail::data

} // namespace sight::io::session

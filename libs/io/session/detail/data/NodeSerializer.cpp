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

#include "NodeSerializer.hpp"

#include <data/Node.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void NodeSerializer::serialize(
    const zip::ArchiveWriter::sptr&,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string&
) const
{
    const auto& node = safeCast<sight::data::Node>(object);

    // Add a version number. Not mandatory, but could help for future release
    writeVersion<sight::data::Node>(tree, 1);

    children["Object"] = node->getObject();

    size_t index = 0;
    for(const auto& input : node->getInputPorts())
    {
        children["Input" + std::to_string(index++)] = input;
    }

    index = 0;
    for(const auto& output : node->getOutputPorts())
    {
        children["Output" + std::to_string(index++)] = output;
    }
}

} // detail::data

} // namespace sight::io::session

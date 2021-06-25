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

#include "ProcessObjectDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/ProcessObject.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr ProcessObjectDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& processObject = safeCast<sight::data::ProcessObject>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::ProcessObject>(tree, 0, 1);

    // Deserialize inputs / outputs
    auto inputs = processObject->getInputs();
    inputs.clear();

    auto outputs = processObject->getOutputs();
    outputs.clear();

    for(const auto& child : children)
    {
        if(child.first[0] == 'I')
        {
            inputs[child.first.substr(1)] = child.second;
        }
        else if(child.first[0] == 'O')
        {
            outputs[child.first.substr(1)] = child.second;
        }
    }

    processObject->setInputs(inputs);
    processObject->setOutputs(outputs);

    return processObject;
}

} // detail::data

} // namespace sight::io::session

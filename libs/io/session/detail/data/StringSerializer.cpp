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

#include "StringSerializer.hpp"

#include <core/crypto/Base64.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void StringSerializer::serialize(
    const zip::ArchiveWriter::sptr&,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>&,
    const core::crypto::secure_string&
) const
{
    const auto& string = IDataSerializer::safeCast<sight::data::String>(object);

    // Add a version number. Not mandatory, but could help for future release
    IDataSerializer::writeVersion<sight::data::String>(tree, 1);

    writeString(tree, "Value", string->getValue());
}

} // detail::data

} // namespace sight::io::session

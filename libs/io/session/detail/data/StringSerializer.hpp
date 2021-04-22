/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#include "io/session/config.hpp"
#include "io/session/detail/data/GenericSerializer.hpp"

#include <data/String.hpp>

namespace sight::io::session
{
namespace detail::data
{

/// Class used to serialize String objects to a session
class StringSerializer : public GenericSerializer<sight::data::String>
{
public:
    SIGHT_DECLARE_CLASS(StringSerializer, GenericSerializer<sight::data::String>)

    /// Delete default copy constructors and assignment operators
    StringSerializer(const StringSerializer&)            = delete;
    StringSerializer(StringSerializer&&)                 = delete;
    StringSerializer& operator=(const StringSerializer&) = delete;
    StringSerializer& operator=(StringSerializer&&)      = delete;

    /// Default destructor. Public to allow unique_ptr
    ~StringSerializer() override = default;

    /// Factories
    static StringSerializer::sptr shared();

    static StringSerializer::uptr unique();

    /// Serialization function
    void serialize(
        const zip::ArchiveWriter::sptr& archive,
        boost::property_tree::ptree& tree,
        const sight::data::Object::csptr& object,
        std::map<std::string, sight::data::Object::csptr>& children,
        const core::crypto::secure_string& password = "") override;

protected:

    /// Default constructor
    StringSerializer() = default;
};

} // namespace detail::data
} // namespace sight::io::session
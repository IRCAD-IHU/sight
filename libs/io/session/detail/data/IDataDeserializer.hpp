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

#include <core/crypto/secure_string.hpp>
#include <core/macros.hpp>

#include <data/Object.hpp>

#include <io/zip/ArchiveReader.hpp>

#include <boost/property_tree/ptree.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Interface class used to deserialize objects from a session
class IDataDeserializer
{
public:

    SIGHT_DECLARE_CLASS(IDataDeserializer);

    /// Delete default copy constructors and assignment operators
    IDataDeserializer(const IDataDeserializer&)            = delete;
    IDataDeserializer(IDataDeserializer&&)                 = delete;
    IDataDeserializer& operator=(const IDataDeserializer&) = delete;
    IDataDeserializer& operator=(IDataDeserializer&&)      = delete;

    /// Default destructor. Public to allow unique_ptr
    virtual ~IDataDeserializer() = default;

    /// Serialization function
    /// @param[in] archive the archive where to read binary files
    /// @param[in] tree the tree where to read trivial object members
    /// @param[in] children a map of data object that belongs to the object
    /// @param[in,out] object the object to deserialize using the archive, the tree and the children map
    /// @param[in] password (optional) the password used for encryption
    virtual sight::data::Object::sptr deserialize(
        const zip::ArchiveReader::sptr& archive,
        const boost::property_tree::ptree& tree,
        const std::map<std::string, sight::data::Object::sptr>& children,
        const sight::data::Object::sptr& object,
        const core::crypto::secure_string& password = ""
    ) const                                         = 0;

protected:

    /// Default constructor
    IDataDeserializer() = default;

    /// Convenience function to safely read strings from a tree
    /// @param[in] tree boost property tree where string data are stored
    /// @param[in] key the string data key
    /// @param[in] password (optional) password used for encryption
    static std::string readString(
        const boost::property_tree::ptree& tree,
        const std::string& key,
        const core::crypto::secure_string& password = ""
    );

    /// Convenience function to read a version from a tree.
    /// Optionally checks the version number, when minVersion or maxVersion > 0.
    /// @param[in] tree boost property tree where version is stored
    template<typename T>
    static int readVersion(
        const boost::property_tree::ptree& tree,
        const int minVersion = 0,
        const int maxVersion = 0
    );

    /// Convenience function to cast and check an object
    /// Mainly to factorize error management
    /// @param[in] object the object to cast to type T
    template<typename T>
    static typename T::sptr safeCast(const sight::data::Object::sptr& object);
};

//------------------------------------------------------------------------------

template<typename T>
int IDataDeserializer::readVersion(
    const boost::property_tree::ptree& tree,
    const int minVersion,
    const int maxVersion
)
{
    // Add a version number. Not mandatory, but could help for future release
    const int version = tree.get<int>(T::classname() + ".version", -1);

    SIGHT_THROW_IF(
        T::classname() << " is not implemented for version '" << version << "'.",
        (minVersion > 0 && minVersion > version) || (maxVersion > 0 && maxVersion < version)
    );

    return version;
}

//------------------------------------------------------------------------------

template<typename T>
typename T::sptr IDataDeserializer::safeCast(const sight::data::Object::sptr& object)
{
    if(object)
    {
        const auto& casted = T::dynamicCast(object);

        SIGHT_THROW_IF(
            "Object '" << object->getClassname() << "' is not a '" << T::classname() << "'",
            casted == nullptr
        );

        return casted;
    }

    return T::New();
}

} // namespace detail::data

} // namespace sight::io::session

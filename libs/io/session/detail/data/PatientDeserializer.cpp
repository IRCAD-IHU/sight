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

#include "PatientDeserializer.hpp"

#include <core/crypto/AES256.hpp>
#include <core/crypto/Base64.hpp>
#include <core/exceptionmacros.hpp>

#include <data/Patient.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr PatientDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>&,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string& password
) const
{
    // Create or reuse the object
    const auto& patient = IDataDeserializer::safeCast<sight::data::Patient>(object);

    // Check version number. Not mandatory, but could help for future release
    IDataDeserializer::readVersion<sight::data::Patient>(tree, 0, 1);

    // Deserialize patient data
    // Even if the session is not password protected, it is still possible to somewhat protect "sensitive" fields from
    // direct reading
    const auto& fieldPassword = password + patient->getUUID().c_str();

    patient->setName(readString(tree, "Name", fieldPassword));
    patient->setPatientId(readString(tree, "PatientId", fieldPassword));
    patient->setBirthdate(readString(tree, "Birthdate", fieldPassword));
    patient->setSex(readString(tree, "Sex", fieldPassword));

    return patient;
}

} // detail::data

} // namespace sight::io::session

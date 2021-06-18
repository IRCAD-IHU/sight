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

#include "PatientSerializer.hpp"

#include <core/crypto/AES256.hpp>
#include <core/crypto/Base64.hpp>

#include <data/Patient.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void PatientSerializer::serialize(
    const zip::ArchiveWriter::sptr&,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>&,
    const core::crypto::secure_string& password
) const
{
    const auto& patient = safeCast<sight::data::Patient>(object);

    // Add a version number. Not mandatory, but could help for future release
    writeVersion<sight::data::Patient>(tree, 1);

    // Serialize patient data
    // Even if the session is not password protected,
    // it is still possible to somewhat protect "sensitive" fields from direct reading
    const auto& fieldPassword = password + patient->getUUID().c_str();

    writeString(tree, "Name", patient->getName(), fieldPassword);
    writeString(tree, "PatientId", patient->getPatientId(), fieldPassword);
    writeString(tree, "Birthdate", patient->getBirthdate(), fieldPassword);
    writeString(tree, "Sex", patient->getSex(), fieldPassword);
}

} // detail::data

} // namespace sight::io::session

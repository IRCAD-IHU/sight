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

/// Factories
PatientSerializer::sptr PatientSerializer::shared()
{
    struct make_shared_enabler final : public PatientSerializer {};
    return std::make_shared< make_shared_enabler >();
}

//------------------------------------------------------------------------------

PatientSerializer::uptr PatientSerializer::unique()
{
    struct make_unique_enabler final : public PatientSerializer {};
    return std::make_unique< make_unique_enabler >();
}

/// Serialization function
void PatientSerializer::serialize(
    const zip::ArchiveWriter::sptr& archive,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string& password)
{
    const auto& patient = sight::data::Patient::dynamicCast(object);
    SIGHT_ASSERT(
        "Object '"
            << (object ? object->getClassname() : sight::data::Object::classname())
            << "' is not a '"
            << sight::data::Patient::classname()
            << "'",
            patient);

    // Serialize patient data
    // Even if the session is not password protected,
    // it is still possible to somewhat protect "sensitive" fields from direct reading
    const auto& fieldPassword = password + patient->getUUID().c_str();

    writeToTree(tree, "Name", patient->getName(), fieldPassword);
    writeToTree(tree, "PatientId", patient->getPatientId(), fieldPassword);
    writeToTree(tree, "Birthdate", patient->getBirthdate(), fieldPassword);
    writeToTree(tree, "Sex", patient->getSex(), fieldPassword);
}

} // detail::data
} // namespace sight::io::session
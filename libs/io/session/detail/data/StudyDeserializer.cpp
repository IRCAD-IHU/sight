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

#include "StudyDeserializer.hpp"

#include <core/crypto/Base64.hpp>
#include <core/exceptionmacros.hpp>

#include <data/Study.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr StudyDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>&,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& study = IDataDeserializer::safeCast<sight::data::Study>(object);

    // Check version number. Not mandatory, but could help for future release
    IDataDeserializer::readVersion<sight::data::Study>(tree, 0, 1);

    study->setInstanceUID(readString(tree, "InstanceUID"));
    study->setStudyID(readString(tree, "StudyID"));
    study->setDate(readString(tree, "Date"));
    study->setTime(readString(tree, "Time"));
    study->setReferringPhysicianName(readString(tree, "ReferringPhysicianName"));
    study->setConsultingPhysicianName(readString(tree, "ConsultingPhysicianName"));
    study->setDescription(readString(tree, "Description"));
    study->setPatientAge(readString(tree, "PatientAge"));
    study->setPatientSize(readString(tree, "PatientSize"));
    study->setPatientWeight(readString(tree, "PatientWeight"));
    study->setPatientBodyMassIndex(readString(tree, "PatientBodyMassIndex"));

    return study;
}

} // detail::data

} // namespace sight::io::session

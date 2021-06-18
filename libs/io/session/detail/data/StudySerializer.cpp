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

#include "StudySerializer.hpp"

#include <core/crypto/Base64.hpp>

#include <data/Study.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void StudySerializer::serialize(
    const zip::ArchiveWriter::sptr&,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>&,
    const core::crypto::secure_string&
) const
{
    const auto& study = IDataSerializer::safeCast<sight::data::Study>(object);

    // Add a version number. Not mandatory, but could help for future release
    IDataSerializer::writeVersion<sight::data::Study>(tree, 1);

    writeString(tree, "InstanceUID", study->getInstanceUID());
    writeString(tree, "StudyID", study->getStudyID());
    writeString(tree, "Date", study->getDate());
    writeString(tree, "Time", study->getTime());
    writeString(tree, "ReferringPhysicianName", study->getReferringPhysicianName());
    writeString(tree, "ConsultingPhysicianName", study->getConsultingPhysicianName());
    writeString(tree, "Description", study->getDescription());
    writeString(tree, "PatientAge", study->getPatientAge());
    writeString(tree, "PatientSize", study->getPatientSize());
    writeString(tree, "PatientWeight", study->getPatientWeight());
    writeString(tree, "PatientBodyMassIndex", study->getPatientBodyMassIndex());
}

} // detail::data

} // namespace sight::io::session

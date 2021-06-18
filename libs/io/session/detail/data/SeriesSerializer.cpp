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

#include "SeriesSerializer.hpp"

#include <core/crypto/Base64.hpp>

#include <data/Equipment.hpp>
#include <data/Patient.hpp>
#include <data/Series.hpp>
#include <data/Study.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void SeriesSerializer::serialize(
    const zip::ArchiveWriter::sptr&,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string&
) const
{
    const auto& series = safeCast<sight::data::Series>(object);

    // Add a version number. Not mandatory, but could help for future release
    writeVersion<sight::data::Series>(tree, 1);

    // Fill children map
    children["Patient"]   = series->getPatient();
    children["Study"]     = series->getStudy();
    children["Equipment"] = series->getEquipment();

    // Serialize patient data
    writeString(tree, "Modality", series->getModality());
    writeString(tree, "InstanceUID", series->getInstanceUID());
    writeString(tree, "Number", series->getNumber());
    writeString(tree, "Laterality", series->getLaterality());
    writeString(tree, "Date", series->getDate());
    writeString(tree, "Time", series->getTime());
    writeString(tree, "ProtocolName", series->getProtocolName());
    writeString(tree, "Description", series->getDescription());
    writeString(tree, "BodyPartExamined", series->getBodyPartExamined());
    writeString(tree, "PatientPosition", series->getPatientPosition());
    writeString(tree, "AnatomicalOrientationType", series->getAnatomicalOrientationType());
    writeString(tree, "PerformedProcedureStepID", series->getPerformedProcedureStepID());
    writeString(tree, "PerformedProcedureStepStartDate", series->getPerformedProcedureStepStartDate());
    writeString(tree, "PerformedProcedureStepStartTime", series->getPerformedProcedureStepStartTime());
    writeString(tree, "PerformedProcedureStepEndDate", series->getPerformedProcedureStepEndDate());
    writeString(tree, "PerformedProcedureStepEndTime", series->getPerformedProcedureStepEndTime());
    writeString(tree, "PerformedProcedureStepDescription", series->getPerformedProcedureStepDescription());
    writeString(tree, "PerformedProcedureComments", series->getPerformedProcedureComments());

    boost::property_tree::ptree namesTree;

    for(const auto& name : series->getPerformingPhysiciansName())
    {
        namesTree.add("PerformingPhysiciansName", core::crypto::to_base64(name));
    }

    tree.add_child("PerformingPhysiciansNames", namesTree);
}

} // detail::data

} // namespace sight::io::session

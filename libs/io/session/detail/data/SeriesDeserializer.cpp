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

#include "SeriesDeserializer.hpp"

#include <core/crypto/Base64.hpp>
#include <core/exceptionmacros.hpp>

#include <data/Equipment.hpp>
#include <data/Patient.hpp>
#include <data/Series.hpp>
#include <data/Study.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr SeriesDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& series = safeCast<sight::data::Series>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::Series>(tree, 0, 1);

    // Set children from map
    series->setPatient(sight::data::Patient::dynamicCast(children.at("Patient")));
    series->setStudy(sight::data::Study::dynamicCast(children.at("Study")));
    series->setEquipment(sight::data::Equipment::dynamicCast(children.at("Equipment")));

    // Deserialize patient data
    series->setModality(readString(tree, "Modality"));
    series->setInstanceUID(readString(tree, "InstanceUID"));
    series->setNumber(readString(tree, "Number"));
    series->setLaterality(readString(tree, "Laterality"));
    series->setDate(readString(tree, "Date"));
    series->setTime(readString(tree, "Time"));
    series->setProtocolName(readString(tree, "ProtocolName"));
    series->setDescription(readString(tree, "Description"));
    series->setBodyPartExamined(readString(tree, "BodyPartExamined"));
    series->setPatientPosition(readString(tree, "PatientPosition"));
    series->setAnatomicalOrientationType(readString(tree, "AnatomicalOrientationType"));
    series->setPerformedProcedureStepID(readString(tree, "PerformedProcedureStepID"));
    series->setPerformedProcedureStepStartDate(readString(tree, "PerformedProcedureStepStartDate"));
    series->setPerformedProcedureStepStartTime(readString(tree, "PerformedProcedureStepStartTime"));
    series->setPerformedProcedureStepEndDate(readString(tree, "PerformedProcedureStepEndDate"));
    series->setPerformedProcedureStepEndTime(readString(tree, "PerformedProcedureStepEndTime"));
    series->setPerformedProcedureStepDescription(readString(tree, "PerformedProcedureStepDescription"));
    series->setPerformedProcedureComments(readString(tree, "PerformedProcedureComments"));

    // Iterate on performingPhysiciansName
    std::vector<std::string> names;

    for(const auto& nameTree : tree.get_child("PerformingPhysiciansNames"))
    {
        const auto& name = core::crypto::from_base64(nameTree.second.get_value<std::string>());
        names.push_back(name);
    }

    series->setPerformingPhysiciansName(names);

    return series;
}

} // detail::data

} // namespace sight::io::session

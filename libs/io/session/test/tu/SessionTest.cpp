/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SessionTest.hpp"

#include <core/crypto/AES256.hpp>
#include <core/data/ActivitySeries.hpp>
#include <core/data/Array.hpp>
#include <core/data/Boolean.hpp>
#include <core/data/CalibrationInfo.hpp>
#include <core/data/Camera.hpp>
#include <core/data/CameraSeries.hpp>
#include <core/data/Composite.hpp>
#include <core/data/Equipment.hpp>
#include <core/data/Float.hpp>
#include <core/data/Image.hpp>
#include <core/data/Integer.hpp>
#include <core/data/iterator/MeshIterators.hpp>
#include <core/data/iterator/MeshIterators.hxx>
#include <core/data/Patient.hpp>
#include <core/data/Point.hpp>
#include <core/data/PointList.hpp>
#include <core/data/Series.hpp>
#include <core/data/String.hpp>
#include <core/data/Study.hpp>
#include <core/data/Vector.hpp>
#include <core/tools/System.hpp>
#include <core/tools/UUID.hpp>

#include <geometry/data/Mesh.hpp>

#include <io/session/detail/SessionDeserializer.hpp>
#include <io/session/detail/SessionSerializer.hpp>
#include <io/session/SessionReader.hpp>
#include <io/session/SessionWriter.hpp>
#include <io/zip/exception/Read.hpp>
#include <io/zip/exception/Write.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Mesh.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::io::session::ut::SessionTest);

static const double DOUBLE_EPSILON = std::numeric_limits<double>::epsilon();
static const float FLOAT_EPSILON   = std::numeric_limits<float>::epsilon();

namespace sight::io::session
{

namespace ut
{

// For UUID::generateUUID();
using core::tools::UUID;

//------------------------------------------------------------------------------

void SessionTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SessionTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SessionTest::booleanTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "booleanTest.zip";

    const bool right = true;
    const bool wrong = false;
    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        // Create the data::Boolean
        auto rightData = data::Boolean::New(right);

        // Add a field
        rightData->setField(fieldName, data::Boolean::New(wrong));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session
        sessionWriter->setObject(rightData);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& rightData = data::Boolean::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(rightData);
        CPPUNIT_ASSERT_EQUAL(right, rightData->getValue());

        // Test field
        const auto& fieldData = data::Boolean::dynamicCast(rightData->getField(fieldName));
        CPPUNIT_ASSERT(fieldData);
        CPPUNIT_ASSERT_EQUAL(wrong, fieldData->getValue());
    }
}

//------------------------------------------------------------------------------

void SessionTest::integerTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "integerTest.zip";

    const std::int64_t answer = 42;
    const std::int64_t satan  = 666;
    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        // Create the data::Integer
        auto answerData = data::Integer::New(answer);

        // Add a field
        answerData->setField(fieldName, data::Integer::New(satan));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session
        sessionWriter->setObject(answerData);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& answerData = data::Integer::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(answerData);
        CPPUNIT_ASSERT_EQUAL(answer, answerData->getValue());

        // Test field
        const auto& fieldData = data::Integer::dynamicCast(answerData->getField(fieldName));
        CPPUNIT_ASSERT(fieldData);
        CPPUNIT_ASSERT_EQUAL(satan, fieldData->getValue());
    }
}

//------------------------------------------------------------------------------

void SessionTest::floatTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "floatTest.zip";

    const float pi     = 3.141592653589793F;
    const float planck = 6.62607015E-10F;
    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        // Create the data::Float
        auto piData = data::Float::New(pi);

        // Add a field
        piData->setField(fieldName, data::Float::New(planck));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session
        sessionWriter->setObject(piData);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& piData = data::Float::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(piData);
        CPPUNIT_ASSERT_EQUAL(pi, piData->getValue());

        // Test field
        const auto& fieldData = data::Float::dynamicCast(piData->getField(fieldName));
        CPPUNIT_ASSERT(fieldData);
        CPPUNIT_ASSERT_EQUAL(planck, fieldData->getValue());
    }
}

//------------------------------------------------------------------------------

void SessionTest::stringTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "stringTest.zip";

    // Create the data::String
    const std::string verlaine(UUID::generateUUID());
    const core::crypto::secure_string verlaine_suite(UUID::generateUUID());
    const core::crypto::secure_string password(UUID::generateUUID());
    const std::string fieldName(UUID::generateUUID());

    // Test serialization
    {
        const auto& verlaineData = data::String::New(verlaine);

        // Add a String field with a pure binary string (encrypted text)
        const auto& secret = core::crypto::encrypt(verlaine_suite, password);
        verlaineData->setField(fieldName, data::String::New(std::string(secret)));

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(verlaineData);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& verlaineData = data::String::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(verlaineData);
        CPPUNIT_ASSERT_EQUAL(verlaine, verlaineData->getValue());

        // Test field
        const auto& fieldData = data::String::dynamicCast(verlaineData->getField(fieldName));
        CPPUNIT_ASSERT(fieldData);
        const auto& decrypted = core::crypto::decrypt(core::crypto::secure_string(fieldData->getValue()), password);
        CPPUNIT_ASSERT_EQUAL(verlaine_suite, decrypted);
    }
}

//------------------------------------------------------------------------------

void SessionTest::circularTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "circularTest.zip";

    const std::string inception(UUID::generateUUID());

    // Test serialization
    {
        // Introduce a circular reference
        auto inceptionData = data::String::New(inception);
        inceptionData->setField(inception, inceptionData);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(inceptionData);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& inceptionData = data::String::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(inceptionData);
        CPPUNIT_ASSERT_EQUAL(inception, inceptionData->getValue());

        // Test field
        const auto& fieldData = data::String::dynamicCast(inceptionData->getField(inception));
        CPPUNIT_ASSERT(fieldData);
        CPPUNIT_ASSERT_EQUAL(inception, fieldData->getValue());
    }
}

//------------------------------------------------------------------------------

void SessionTest::compositeTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "compositeTest.zip";

    const std::string stringValue(UUID::generateUUID());
    const std::int64_t integerValue = 42;
    const bool booleanValue         = true;
    const float floatValue          = 3.141592653589793F;

    // Test serialization
    {
        // Create composite
        auto composite = data::Composite::New();
        (*composite)[data::String::classname()]    = data::String::New(stringValue);
        (*composite)[data::Integer::classname()]   = data::Integer::New(integerValue);
        (*composite)[data::Boolean::classname()]   = data::Boolean::New(booleanValue);
        (*composite)[data::Float::classname()]     = data::Float::New(floatValue);
        (*composite)[data::Composite::classname()] = composite;

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(composite);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& composite = data::Composite::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(composite);

        const auto& stringData = data::String::dynamicCast((*composite)[data::String::classname()]);
        CPPUNIT_ASSERT(stringData);
        CPPUNIT_ASSERT_EQUAL(stringValue, stringData->getValue());

        const auto& integerData = data::Integer::dynamicCast((*composite)[data::Integer::classname()]);
        CPPUNIT_ASSERT(integerData);
        CPPUNIT_ASSERT_EQUAL(integerValue, integerData->getValue());

        const auto& booleanData = data::Boolean::dynamicCast((*composite)[data::Boolean::classname()]);
        CPPUNIT_ASSERT(booleanData);
        CPPUNIT_ASSERT_EQUAL(booleanValue, booleanData->getValue());

        const auto& floatData = data::Float::dynamicCast((*composite)[data::Float::classname()]);
        CPPUNIT_ASSERT(floatData);
        CPPUNIT_ASSERT_EQUAL(floatValue, floatData->getValue());

        const auto& compositeData = data::Composite::dynamicCast((*composite)[data::Composite::classname()]);
        CPPUNIT_ASSERT(compositeData);
        const auto& compositeStringData = data::String::dynamicCast((*compositeData)[data::String::classname()]);
        CPPUNIT_ASSERT(compositeStringData);
        CPPUNIT_ASSERT_EQUAL(stringValue, compositeStringData->getValue());
    }
}

//------------------------------------------------------------------------------

void SessionTest::meshTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "meshTest.zip";

    // Create a test mesh
    const auto& originalMesh = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleQuadMesh(originalMesh);
    geometry::data::Mesh::shakePoint(originalMesh);
    geometry::data::Mesh::colorizeMeshPoints(originalMesh);
    geometry::data::Mesh::colorizeMeshCells(originalMesh);
    geometry::data::Mesh::generatePointNormals(originalMesh);
    geometry::data::Mesh::generateCellNormals(originalMesh);
    originalMesh->adjustAllocatedMemory();

    // Test serialization
    {
        const auto& mesh = data::Mesh::New();
        mesh->deepCopy(originalMesh);
        mesh->adjustAllocatedMemory();

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(mesh);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& mesh = data::Mesh::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(mesh);

        CPPUNIT_ASSERT_EQUAL(originalMesh->getAttributes(), mesh->getAttributes());
        CPPUNIT_ASSERT_EQUAL(originalMesh->getNumberOfCells(), mesh->getNumberOfCells());
        CPPUNIT_ASSERT_EQUAL(originalMesh->getNumberOfPoints(), mesh->getNumberOfPoints());
        CPPUNIT_ASSERT_EQUAL(originalMesh->getCellDataSize(), mesh->getCellDataSize());
        CPPUNIT_ASSERT_EQUAL(originalMesh->getDataSizeInBytes(), mesh->getDataSizeInBytes());

        auto originalLock       = originalMesh->lock();
        auto originalIt         = originalMesh->begin<data::iterator::PointIterator>();
        const auto& originalEnd = originalMesh->end<data::iterator::PointIterator>();

        auto meshLock       = mesh->lock();
        auto meshIt         = mesh->begin<data::iterator::PointIterator>();
        const auto& meshEnd = mesh->end<data::iterator::PointIterator>();

        for( ; originalIt != originalEnd && meshIt != meshEnd ; ++originalIt, ++meshIt)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(originalIt->point->x, meshIt->point->x, FLOAT_EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(originalIt->point->y, meshIt->point->y, FLOAT_EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(originalIt->point->z, meshIt->point->z, FLOAT_EPSILON);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(
                originalIt->normal->nx,
                meshIt->normal->nx,
                FLOAT_EPSILON
            );
            CPPUNIT_ASSERT_DOUBLES_EQUAL(
                originalIt->normal->ny,
                meshIt->normal->ny,
                FLOAT_EPSILON
            );
            CPPUNIT_ASSERT_DOUBLES_EQUAL(
                originalIt->normal->nz,
                meshIt->normal->nz,
                FLOAT_EPSILON
            );
        }
    }
}

//------------------------------------------------------------------------------

void SessionTest::equipmentTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "equipmentTest.zip";

    const std::string institutionName(UUID::generateUUID());

    // Test serialization
    {
        const auto& equipment = data::Equipment::New();
        equipment->setInstitutionName(institutionName);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(equipment);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& equipment = data::Equipment::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(equipment);

        CPPUNIT_ASSERT_EQUAL(institutionName, equipment->getInstitutionName());
    }
}

//------------------------------------------------------------------------------

void SessionTest::patientTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "patientTest.zip";

    const std::string name(UUID::generateUUID());
    const std::string patientId(UUID::generateUUID());
    const std::string birthdate(UUID::generateUUID());
    const std::string sex(UUID::generateUUID());

    // Test serialization
    {
        const auto& patient = data::Patient::New();
        patient->setName(name);
        patient->setPatientId(patientId);
        patient->setBirthdate(birthdate);
        patient->setSex(sex);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(patient);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& patient = data::Patient::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(patient);

        CPPUNIT_ASSERT_EQUAL(name, patient->getName());
        CPPUNIT_ASSERT_EQUAL(patientId, patient->getPatientId());
        CPPUNIT_ASSERT_EQUAL(birthdate, patient->getBirthdate());
        CPPUNIT_ASSERT_EQUAL(sex, patient->getSex());
    }
}

//------------------------------------------------------------------------------

void SessionTest::studyTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "studyTest.zip";

    const std::string instanceUID(UUID::generateUUID());
    const std::string studyID(UUID::generateUUID());
    const std::string date(UUID::generateUUID());
    const std::string time(UUID::generateUUID());
    const std::string referringPhysicianName(UUID::generateUUID());
    const std::string consultingPhysicianName(UUID::generateUUID());
    const std::string description(UUID::generateUUID());
    const std::string patientAge(UUID::generateUUID());
    const std::string patientSize(UUID::generateUUID());
    const std::string patientWeight(UUID::generateUUID());
    const std::string patientBodyMassIndex(UUID::generateUUID());

    // Test serialization
    {
        const auto& study = data::Study::New();

        study->setInstanceUID(instanceUID);
        study->setStudyID(studyID);
        study->setDate(date);
        study->setTime(time);
        study->setReferringPhysicianName(referringPhysicianName);
        study->setConsultingPhysicianName(consultingPhysicianName);
        study->setDescription(description);
        study->setPatientAge(patientAge);
        study->setPatientSize(patientSize);
        study->setPatientWeight(patientWeight);
        study->setPatientBodyMassIndex(patientBodyMassIndex);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(study);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& study = data::Study::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(study);

        CPPUNIT_ASSERT_EQUAL(instanceUID, study->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(studyID, study->getStudyID());
        CPPUNIT_ASSERT_EQUAL(date, study->getDate());
        CPPUNIT_ASSERT_EQUAL(time, study->getTime());
        CPPUNIT_ASSERT_EQUAL(referringPhysicianName, study->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(consultingPhysicianName, study->getConsultingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(description, study->getDescription());
        CPPUNIT_ASSERT_EQUAL(patientAge, study->getPatientAge());
        CPPUNIT_ASSERT_EQUAL(patientSize, study->getPatientSize());
        CPPUNIT_ASSERT_EQUAL(patientWeight, study->getPatientWeight());
        CPPUNIT_ASSERT_EQUAL(patientBodyMassIndex, study->getPatientBodyMassIndex());
    }
}

//------------------------------------------------------------------------------

void SessionTest::seriesTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "seriesTest.zip";

    /// Test vector
    const std::string name(UUID::generateUUID());
    const std::string patientId(UUID::generateUUID());
    const std::string birthdate(UUID::generateUUID());
    const std::string sex(UUID::generateUUID());

    const auto& patient = data::Patient::New();
    patient->setName(name);
    patient->setPatientId(patientId);
    patient->setBirthdate(birthdate);
    patient->setSex(sex);

    const std::string instanceUID(UUID::generateUUID());
    const std::string studyID(UUID::generateUUID());
    const std::string date(UUID::generateUUID());
    const std::string time(UUID::generateUUID());
    const std::string referringPhysicianName(UUID::generateUUID());
    const std::string consultingPhysicianName(UUID::generateUUID());
    const std::string description(UUID::generateUUID());
    const std::string patientAge(UUID::generateUUID());
    const std::string patientSize(UUID::generateUUID());
    const std::string patientWeight(UUID::generateUUID());
    const std::string patientBodyMassIndex(UUID::generateUUID());

    const auto& study = data::Study::New();
    study->setInstanceUID(instanceUID);
    study->setStudyID(studyID);
    study->setDate(date);
    study->setTime(time);
    study->setReferringPhysicianName(referringPhysicianName);
    study->setConsultingPhysicianName(consultingPhysicianName);
    study->setDescription(description);
    study->setPatientAge(patientAge);
    study->setPatientSize(patientSize);
    study->setPatientWeight(patientWeight);
    study->setPatientBodyMassIndex(patientBodyMassIndex);

    const std::string institutionName(UUID::generateUUID());

    const auto& equipment = data::Equipment::New();
    equipment->setInstitutionName(institutionName);

    const std::string modality(UUID::generateUUID());
    const std::string number(UUID::generateUUID());
    const std::string laterality(UUID::generateUUID());
    const std::vector<std::string> performingPhysiciansName =
    {UUID::generateUUID(), UUID::generateUUID(), UUID::generateUUID()};
    const std::string protocolName(UUID::generateUUID());
    const std::string bodyPartExamined(UUID::generateUUID());
    const std::string patientPosition(UUID::generateUUID());
    const std::string anatomicalOrientationType(UUID::generateUUID());
    const std::string performdedProcedureStepID(UUID::generateUUID());
    const std::string performedProcedureStepStartDate(UUID::generateUUID());
    const std::string performedProcedureStepStartTime(UUID::generateUUID());
    const std::string performedProcedureStepEndDate(UUID::generateUUID());
    const std::string performedProcedureStepEndTime(UUID::generateUUID());
    const std::string performedProcedureStepDescription(UUID::generateUUID());
    const std::string performedProcedureComments(UUID::generateUUID());

    // Test serialization
    {
        // Set "child" attributes
        const auto& seriesPatient = data::Patient::New();
        seriesPatient->deepCopy(patient);

        const auto& seriesStudy = data::Study::New();
        seriesStudy->deepCopy(study);

        const auto& seriesEquipment = data::Equipment::New();
        seriesEquipment->deepCopy(equipment);

        const auto& series = data::Series::New();
        series->setPatient(seriesPatient);
        series->setStudy(seriesStudy);
        series->setEquipment(seriesEquipment);

        // Fill trivial attributes
        series->setModality(modality);
        series->setInstanceUID(instanceUID);
        series->setNumber(number);
        series->setLaterality(laterality);
        series->setDate(date);
        series->setTime(time);
        series->setPerformingPhysiciansName(performingPhysiciansName);
        series->setProtocolName(protocolName);
        series->setDescription(description);
        series->setBodyPartExamined(bodyPartExamined);
        series->setPatientPosition(patientPosition);
        series->setAnatomicalOrientationType(anatomicalOrientationType);
        series->setPerformedProcedureStepID(performdedProcedureStepID);
        series->setPerformedProcedureStepStartDate(performedProcedureStepStartDate);
        series->setPerformedProcedureStepStartTime(performedProcedureStepStartTime);
        series->setPerformedProcedureStepEndDate(performedProcedureStepEndDate);
        series->setPerformedProcedureStepEndTime(performedProcedureStepEndTime);
        series->setPerformedProcedureStepDescription(performedProcedureStepDescription);
        series->setPerformedProcedureComments(performedProcedureComments);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(series);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& series = data::Series::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(series);

        // Equipment
        CPPUNIT_ASSERT_EQUAL(institutionName, series->getEquipment()->getInstitutionName());

        // Study
        const auto& seriesStudy = series->getStudy();
        CPPUNIT_ASSERT_EQUAL(instanceUID, seriesStudy->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(studyID, seriesStudy->getStudyID());
        CPPUNIT_ASSERT_EQUAL(date, seriesStudy->getDate());
        CPPUNIT_ASSERT_EQUAL(time, seriesStudy->getTime());
        CPPUNIT_ASSERT_EQUAL(referringPhysicianName, seriesStudy->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(consultingPhysicianName, seriesStudy->getConsultingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(description, seriesStudy->getDescription());
        CPPUNIT_ASSERT_EQUAL(patientAge, seriesStudy->getPatientAge());
        CPPUNIT_ASSERT_EQUAL(patientSize, seriesStudy->getPatientSize());
        CPPUNIT_ASSERT_EQUAL(patientWeight, seriesStudy->getPatientWeight());
        CPPUNIT_ASSERT_EQUAL(patientBodyMassIndex, seriesStudy->getPatientBodyMassIndex());

        // Patient
        const auto& seriesPatient = series->getPatient();
        CPPUNIT_ASSERT_EQUAL(name, seriesPatient->getName());
        CPPUNIT_ASSERT_EQUAL(patientId, seriesPatient->getPatientId());
        CPPUNIT_ASSERT_EQUAL(birthdate, seriesPatient->getBirthdate());
        CPPUNIT_ASSERT_EQUAL(sex, seriesPatient->getSex());

        // Trivial attributes
        CPPUNIT_ASSERT_EQUAL(modality, series->getModality());
        CPPUNIT_ASSERT_EQUAL(instanceUID, series->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(number, series->getNumber());
        CPPUNIT_ASSERT_EQUAL(laterality, series->getLaterality());
        CPPUNIT_ASSERT_EQUAL(date, series->getDate());
        CPPUNIT_ASSERT_EQUAL(time, series->getTime());

        const auto& seriesName = series->getPerformingPhysiciansName();

        for(std::size_t i = 0 ; i < performingPhysiciansName.size() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(performingPhysiciansName[i], seriesName[i]);
        }

        CPPUNIT_ASSERT_EQUAL(protocolName, series->getProtocolName());
        CPPUNIT_ASSERT_EQUAL(description, series->getDescription());
        CPPUNIT_ASSERT_EQUAL(bodyPartExamined, series->getBodyPartExamined());
        CPPUNIT_ASSERT_EQUAL(patientPosition, series->getPatientPosition());
        CPPUNIT_ASSERT_EQUAL(anatomicalOrientationType, series->getAnatomicalOrientationType());
        CPPUNIT_ASSERT_EQUAL(performdedProcedureStepID, series->getPerformedProcedureStepID());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartDate, series->getPerformedProcedureStepStartDate());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartTime, series->getPerformedProcedureStepStartTime());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndDate, series->getPerformedProcedureStepEndDate());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndTime, series->getPerformedProcedureStepEndTime());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepDescription, series->getPerformedProcedureStepDescription());
        CPPUNIT_ASSERT_EQUAL(performedProcedureComments, series->getPerformedProcedureComments());
    }
}

//------------------------------------------------------------------------------

void SessionTest::activitySeriesTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "activitySeriesTest.zip";

    // Test vector
    const std::string stringValue(UUID::generateUUID());
    const std::int64_t integerValue = 42;
    const bool booleanValue         = true;
    const float floatValue          = 3.141592653589793F;

    // Create composite
    const auto& composite = data::Composite::New();
    (*composite)[data::String::classname()]    = data::String::New(stringValue);
    (*composite)[data::Integer::classname()]   = data::Integer::New(integerValue);
    (*composite)[data::Boolean::classname()]   = data::Boolean::New(booleanValue);
    (*composite)[data::Float::classname()]     = data::Float::New(floatValue);
    (*composite)[data::Composite::classname()] = composite;

    const std::string activityConfigId(UUID::generateUUID());

    const std::string name(UUID::generateUUID());
    const std::string patientId(UUID::generateUUID());
    const std::string birthdate(UUID::generateUUID());
    const std::string sex(UUID::generateUUID());

    const auto& patient = data::Patient::New();
    patient->setName(name);
    patient->setPatientId(patientId);
    patient->setBirthdate(birthdate);
    patient->setSex(sex);

    const std::string instanceUID(UUID::generateUUID());
    const std::string studyID(UUID::generateUUID());
    const std::string date(UUID::generateUUID());
    const std::string time(UUID::generateUUID());
    const std::string referringPhysicianName(UUID::generateUUID());
    const std::string consultingPhysicianName(UUID::generateUUID());
    const std::string description(UUID::generateUUID());
    const std::string patientAge(UUID::generateUUID());
    const std::string patientSize(UUID::generateUUID());
    const std::string patientWeight(UUID::generateUUID());
    const std::string patientBodyMassIndex(UUID::generateUUID());

    const auto& study = data::Study::New();
    study->setInstanceUID(instanceUID);
    study->setStudyID(studyID);
    study->setDate(date);
    study->setTime(time);
    study->setReferringPhysicianName(referringPhysicianName);
    study->setConsultingPhysicianName(consultingPhysicianName);
    study->setDescription(description);
    study->setPatientAge(patientAge);
    study->setPatientSize(patientSize);
    study->setPatientWeight(patientWeight);
    study->setPatientBodyMassIndex(patientBodyMassIndex);

    const std::string institutionName(UUID::generateUUID());

    const auto& equipment = data::Equipment::New();
    equipment->setInstitutionName(institutionName);

    const std::string modality(UUID::generateUUID());
    const std::string number(UUID::generateUUID());
    const std::string laterality(UUID::generateUUID());
    const std::vector<std::string> performingPhysiciansName =
    {UUID::generateUUID(), UUID::generateUUID(), UUID::generateUUID()};
    const std::string protocolName(UUID::generateUUID());
    const std::string bodyPartExamined(UUID::generateUUID());
    const std::string patientPosition(UUID::generateUUID());
    const std::string anatomicalOrientationType(UUID::generateUUID());
    const std::string performdedProcedureStepID(UUID::generateUUID());
    const std::string performedProcedureStepStartDate(UUID::generateUUID());
    const std::string performedProcedureStepStartTime(UUID::generateUUID());
    const std::string performedProcedureStepEndDate(UUID::generateUUID());
    const std::string performedProcedureStepEndTime(UUID::generateUUID());
    const std::string performedProcedureStepDescription(UUID::generateUUID());
    const std::string performedProcedureComments(UUID::generateUUID());

    // Test serialization
    {
        const auto& series = data::ActivitySeries::New();

        // Set "child" attributes
        const auto& seriesComposite = data::Composite::New();
        seriesComposite->deepCopy(composite);

        // For fun...
        (*seriesComposite)[data::ActivitySeries::classname()] = series;

        const auto& seriesPatient = data::Patient::New();
        seriesPatient->deepCopy(patient);

        const auto& seriesStudy = data::Study::New();
        seriesStudy->deepCopy(study);

        const auto& seriesEquipment = data::Equipment::New();
        seriesEquipment->deepCopy(equipment);

        series->setPatient(seriesPatient);
        series->setStudy(seriesStudy);
        series->setEquipment(seriesEquipment);
        series->setData(seriesComposite);

        // Fill trivial attributes
        series->setActivityConfigId(activityConfigId);
        series->setModality(modality);
        series->setInstanceUID(instanceUID);
        series->setNumber(number);
        series->setLaterality(laterality);
        series->setDate(date);
        series->setTime(time);
        series->setPerformingPhysiciansName(performingPhysiciansName);
        series->setProtocolName(protocolName);
        series->setDescription(description);
        series->setBodyPartExamined(bodyPartExamined);
        series->setPatientPosition(patientPosition);
        series->setAnatomicalOrientationType(anatomicalOrientationType);
        series->setPerformedProcedureStepID(performdedProcedureStepID);
        series->setPerformedProcedureStepStartDate(performedProcedureStepStartDate);
        series->setPerformedProcedureStepStartTime(performedProcedureStepStartTime);
        series->setPerformedProcedureStepEndDate(performedProcedureStepEndDate);
        series->setPerformedProcedureStepEndTime(performedProcedureStepEndTime);
        series->setPerformedProcedureStepDescription(performedProcedureStepDescription);
        series->setPerformedProcedureComments(performedProcedureComments);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(series);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& series = data::ActivitySeries::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(series);

        // Composite
        const auto& seriesComposite = series->getData();
        CPPUNIT_ASSERT(seriesComposite);

        const auto& stringData = data::String::dynamicCast((*seriesComposite)[data::String::classname()]);
        CPPUNIT_ASSERT(stringData);
        CPPUNIT_ASSERT_EQUAL(stringValue, stringData->getValue());

        const auto integerData = data::Integer::dynamicCast((*seriesComposite)[data::Integer::classname()]);
        CPPUNIT_ASSERT(integerData);
        CPPUNIT_ASSERT_EQUAL(integerValue, integerData->getValue());

        const auto& booleanData = data::Boolean::dynamicCast((*seriesComposite)[data::Boolean::classname()]);
        CPPUNIT_ASSERT(booleanData);
        CPPUNIT_ASSERT_EQUAL(booleanValue, booleanData->getValue());

        const auto& floatData = data::Float::dynamicCast((*seriesComposite)[data::Float::classname()]);
        CPPUNIT_ASSERT(floatData);
        CPPUNIT_ASSERT_EQUAL(floatValue, floatData->getValue());

        const auto& compositeData = data::Composite::dynamicCast((*seriesComposite)[data::Composite::classname()]);
        CPPUNIT_ASSERT(compositeData);

        const auto& compositeStringData = data::String::dynamicCast((*compositeData)[data::String::classname()]);
        CPPUNIT_ASSERT(compositeStringData);
        CPPUNIT_ASSERT_EQUAL(stringValue, compositeStringData->getValue());

        // Special circular inclusion test
        const auto& activitySeriesData =
            data::ActivitySeries::dynamicCast((*seriesComposite)[data::ActivitySeries::classname()]);
        CPPUNIT_ASSERT(activitySeriesData);
        CPPUNIT_ASSERT_EQUAL(activityConfigId, activitySeriesData->getActivityConfigId());

        // Equipment
        CPPUNIT_ASSERT_EQUAL(institutionName, series->getEquipment()->getInstitutionName());

        // Study
        const auto& seriesStudy = series->getStudy();
        CPPUNIT_ASSERT_EQUAL(instanceUID, seriesStudy->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(studyID, seriesStudy->getStudyID());
        CPPUNIT_ASSERT_EQUAL(date, seriesStudy->getDate());
        CPPUNIT_ASSERT_EQUAL(time, seriesStudy->getTime());
        CPPUNIT_ASSERT_EQUAL(referringPhysicianName, seriesStudy->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(consultingPhysicianName, seriesStudy->getConsultingPhysicianName());
        CPPUNIT_ASSERT_EQUAL(description, seriesStudy->getDescription());
        CPPUNIT_ASSERT_EQUAL(patientAge, seriesStudy->getPatientAge());
        CPPUNIT_ASSERT_EQUAL(patientSize, seriesStudy->getPatientSize());
        CPPUNIT_ASSERT_EQUAL(patientWeight, seriesStudy->getPatientWeight());
        CPPUNIT_ASSERT_EQUAL(patientBodyMassIndex, seriesStudy->getPatientBodyMassIndex());

        // Patient
        const auto& seriesPatient = series->getPatient();
        CPPUNIT_ASSERT_EQUAL(name, seriesPatient->getName());
        CPPUNIT_ASSERT_EQUAL(patientId, seriesPatient->getPatientId());
        CPPUNIT_ASSERT_EQUAL(birthdate, seriesPatient->getBirthdate());
        CPPUNIT_ASSERT_EQUAL(sex, seriesPatient->getSex());

        // Trivial attributes
        CPPUNIT_ASSERT_EQUAL(modality, series->getModality());
        CPPUNIT_ASSERT_EQUAL(instanceUID, series->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(number, series->getNumber());
        CPPUNIT_ASSERT_EQUAL(laterality, series->getLaterality());
        CPPUNIT_ASSERT_EQUAL(date, series->getDate());
        CPPUNIT_ASSERT_EQUAL(time, series->getTime());

        const auto& seriesName = series->getPerformingPhysiciansName();

        for(std::size_t i = 0 ; i < performingPhysiciansName.size() ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(performingPhysiciansName[i], seriesName[i]);
        }

        CPPUNIT_ASSERT_EQUAL(protocolName, series->getProtocolName());
        CPPUNIT_ASSERT_EQUAL(description, series->getDescription());
        CPPUNIT_ASSERT_EQUAL(bodyPartExamined, series->getBodyPartExamined());
        CPPUNIT_ASSERT_EQUAL(patientPosition, series->getPatientPosition());
        CPPUNIT_ASSERT_EQUAL(anatomicalOrientationType, series->getAnatomicalOrientationType());
        CPPUNIT_ASSERT_EQUAL(performdedProcedureStepID, series->getPerformedProcedureStepID());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartDate, series->getPerformedProcedureStepStartDate());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepStartTime, series->getPerformedProcedureStepStartTime());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndDate, series->getPerformedProcedureStepEndDate());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepEndTime, series->getPerformedProcedureStepEndTime());
        CPPUNIT_ASSERT_EQUAL(performedProcedureStepDescription, series->getPerformedProcedureStepDescription());
        CPPUNIT_ASSERT_EQUAL(performedProcedureComments, series->getPerformedProcedureComments());
    }
}

//------------------------------------------------------------------------------

void SessionTest::arrayTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "arrayTest.zip";

    // Test vector
    const std::array<std::array<std::uint8_t, 3>, 4> testVector = {{
        {0, 1, 1},
        {2, 3, 5},
        {8, 13, 21},
        {34, 55, 89}
    }
    };

    // Test serialization
    {
        // Create the array
        data::Array::sptr array    = data::Array::New();
        data::Array::SizeType size = {3, 4};
        array->resize(size, core::tools::Type::s_UINT8, true);

        // Fill
        data::Array::Iterator<std::uint8_t> it = array->begin<std::uint8_t>();

        for(const auto& row : testVector)
        {
            for(const auto cell : row)
            {
                *it = cell;
                ++it;
            }
        }

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(array);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& array = data::Array::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(array);

        data::Array::Iterator<std::uint8_t> it = array->begin<std::uint8_t>();

        for(const auto& row : testVector)
        {
            for(const auto cell : row)
            {
                CPPUNIT_ASSERT_EQUAL(cell, *it);
                ++it;
            }
        }
    }
}

//------------------------------------------------------------------------------

void SessionTest::imageTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "imageTest.zip";

    // Test vector
    const auto testVector =
        []
        {
            std::array<std::array<std::array<std::array<std::uint8_t, 3>, 4>, 5>, 6> tmp;
            std::uint8_t index = 0;

            for(auto& x : tmp)
            {
                for(auto& y : x)
                {
                    for(auto& z : y)
                    {
                        for(auto& component : z)
                        {
                            component = index++;
                        }
                    }
                }
            }

            return tmp;
        }();

    // Test serialization
    {
        // Create the image
        auto image = data::Image::New();

        const core::tools::Type TYPE = core::tools::Type::s_UINT8;
        const data::Image::Size SIZE = {4, 5, 6};

        image->resize(SIZE, TYPE, data::Image::PixelFormat::RGB);

        auto it = image->begin<data::iterator::RGB>();

        for(auto& x : testVector)
        {
            for(auto& y : x)
            {
                for(auto& z : y)
                {
                    it->r = z.at(0);
                    it->g = z.at(1);
                    it->b = z.at(2);
                    ++it;
                }
            }
        }

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(image);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test values
        const auto& image = data::Image::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(image);

        auto it = image->begin<data::iterator::RGB>();

        for(auto& x : testVector)
        {
            for(auto& y : x)
            {
                for(auto& z : y)
                {
                    CPPUNIT_ASSERT_EQUAL(z.at(0), it->r);
                    CPPUNIT_ASSERT_EQUAL(z.at(1), it->g);
                    CPPUNIT_ASSERT_EQUAL(z.at(2), it->b);
                    ++it;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void SessionTest::vectorTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "vectorTest.zip";

    const std::string stringValue(UUID::generateUUID());
    const std::int64_t integerValue = 42;
    const bool booleanValue         = true;
    const float floatValue          = 3.141592653589793F;

    // Test serialization
    {
        // Create vector
        auto vector     = data::Vector::New();
        auto& container = vector->getContainer();
        container.push_back(data::String::New(stringValue));
        container.push_back(data::Integer::New(integerValue));
        container.push_back(data::Boolean::New(booleanValue));
        container.push_back(data::Float::New(floatValue));
        container.push_back(vector);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(vector);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& vector = data::Vector::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(vector);

        const auto& stringData = data::String::dynamicCast((*vector)[0]);
        CPPUNIT_ASSERT(stringData);
        CPPUNIT_ASSERT_EQUAL(stringValue, stringData->getValue());

        const auto& integerData = data::Integer::dynamicCast((*vector)[1]);
        CPPUNIT_ASSERT(integerData);
        CPPUNIT_ASSERT_EQUAL(integerValue, integerData->getValue());

        const auto& booleanData = data::Boolean::dynamicCast((*vector)[2]);
        CPPUNIT_ASSERT(booleanData);
        CPPUNIT_ASSERT_EQUAL(booleanValue, booleanData->getValue());

        const auto& floatData = data::Float::dynamicCast((*vector)[3]);
        CPPUNIT_ASSERT(floatData);
        CPPUNIT_ASSERT_EQUAL(floatValue, floatData->getValue());

        const auto& vectorData = data::Vector::dynamicCast((*vector)[4]);
        CPPUNIT_ASSERT(vectorData);
        const auto& vectorStringData = data::String::dynamicCast((*vectorData)[0]);
        CPPUNIT_ASSERT(vectorStringData);
        CPPUNIT_ASSERT_EQUAL(stringValue, vectorStringData->getValue());
    }
}

//------------------------------------------------------------------------------

void SessionTest::pointTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "pointTest.zip";

    const std::array<double, 3> coordinates = {
        0.111111111111,
        0.222222222222,
        0.333333333333
    };

    // Test serialization
    {
        // Create vector
        auto point = data::Point::New();
        point->setCoord(coordinates);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(point);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& point = data::Point::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(point);

        const auto& coords = point->getCoord();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(coordinates[0], coords[0], DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(coordinates[1], coords[1], DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(coordinates[2], coords[2], DOUBLE_EPSILON);
    }
}

//------------------------------------------------------------------------------

void SessionTest::pointListTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "pointListTest.zip";

    const std::array<double, 3> coordinates1 = {
        0.111111111111,
        0.222222222222,
        0.333333333333
    };

    const std::array<double, 3> coordinates2 = {
        0.444444444444,
        0.555555555555,
        0.666666666666
    };

    const std::array<double, 3> coordinates3 = {
        0.777777777777,
        0.888888888888,
        0.999999999999
    };

    // Test serialization
    {
        // Create vector
        std::vector<data::Point::sptr> points;

        auto point1 = data::Point::New();
        point1->setCoord(coordinates1);
        points.push_back(point1);

        auto point2 = data::Point::New();
        point2->setCoord(coordinates2);
        points.push_back(point2);

        auto point3 = data::Point::New();
        point3->setCoord(coordinates3);
        points.push_back(point3);

        auto pointList = data::PointList::New();
        pointList->setPoints(points);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(pointList);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& pointList = data::PointList::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(pointList);

        const auto& points  = pointList->getPoints();
        const auto& point1  = points[0];
        const auto& coords1 = point1->getCoord();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(coordinates1[0], coords1[0], DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(coordinates1[1], coords1[1], DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(coordinates1[2], coords1[2], DOUBLE_EPSILON);

        const auto& point2  = points[1];
        const auto& coords2 = point2->getCoord();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(coordinates2[0], coords2[0], DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(coordinates2[1], coords2[1], DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(coordinates2[2], coords2[2], DOUBLE_EPSILON);

        const auto& point3  = points[2];
        const auto& coords3 = point3->getCoord();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(coordinates3[0], coords3[0], DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(coordinates3[1], coords3[1], DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(coordinates3[2], coords3[2], DOUBLE_EPSILON);
    }
}

//------------------------------------------------------------------------------

void SessionTest::calibrationInfoTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "calibrationInfoTest.zip";

    // Images data
    const auto imagesData =
        []
        {
            std::array<std::array<std::array<std::array<std::uint8_t, 3>, 3>, 3>, 3> tmp;
            std::uint8_t index = 0;

            for(auto& image : tmp)
            {
                for(auto& x : image)
                {
                    for(auto& y : x)
                    {
                        for(auto& component : y)
                        {
                            component = index++;
                        }
                    }
                }
            }

            return tmp;
        }();

    // PointLists data
    const auto pointListsData =
        []
        {
            std::array<std::array<std::array<double, 3>, 3>, 3> tmp;
            double pi = 3.141592653589793;

            for(auto& pointList : tmp)
            {
                for(auto& point : pointList)
                {
                    for(auto& coord : point)
                    {
                        coord = pi++;
                    }
                }
            }

            return tmp;
        }();

    // Test serialization
    {
        auto calibrationInfo = data::CalibrationInfo::New();

        for(std::size_t i = 0 ; i < 3 ; ++i)
        {
            // Create the image
            auto image = data::Image::New();

            const core::tools::Type TYPE = core::tools::Type::s_UINT8;
            const data::Image::Size SIZE = {3, 3};

            image->resize(SIZE, TYPE, data::Image::PixelFormat::RGB);
            auto it = image->begin<data::iterator::RGB>();

            for(auto& x : imagesData[i])
            {
                for(auto& y : x)
                {
                    it->r = y.at(0);
                    it->g = y.at(1);
                    it->b = y.at(2);
                    ++it;
                }
            }

            auto pointList = data::PointList::New();

            // Create the point list
            for(auto& pointData : pointListsData[i])
            {
                auto point = data::Point::New();
                point->setCoord(pointData);
                pointList->pushBack(point);
            }

            calibrationInfo->addRecord(image, pointList);
        }

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(calibrationInfo);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& calibrationInfo = data::CalibrationInfo::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(calibrationInfo);

        for(std::size_t i = 0 ; i < 3 ; ++i)
        {
            const auto& image = calibrationInfo->getImage(i);
            auto it           = image->begin<data::iterator::RGB>();

            for(auto& x : imagesData[i])
            {
                for(auto& y : x)
                {
                    CPPUNIT_ASSERT_EQUAL(y.at(0), it->r);
                    CPPUNIT_ASSERT_EQUAL(y.at(1), it->g);
                    CPPUNIT_ASSERT_EQUAL(y.at(2), it->b);
                    ++it;
                }
            }

            const auto& pointList = calibrationInfo->getPointList(image);
            const auto& points    = pointList->getPoints();

            for(std::size_t j = 0 ; j < 3 ; ++j)
            {
                const auto& coord = points[j]->getCoord();
                CPPUNIT_ASSERT_DOUBLES_EQUAL(pointListsData[i][j][0], coord[0], DOUBLE_EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(pointListsData[i][j][1], coord[1], DOUBLE_EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(pointListsData[i][j][2], coord[2], DOUBLE_EPSILON);
            }
        }
    }
}

//------------------------------------------------------------------------------

void SessionTest::cameraTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "cameraTest.zip";

    // Test data
    const size_t width  = 111;
    const size_t height = 222;

    const double fx = 0.0000001;
    const double fy = 0.0000002;
    const double cx = 0.0000003;
    const double cy = 0.0000004;

    const double k1 = 0.0000005;
    const double k2 = 0.0000006;
    const double p1 = 0.0000007;
    const double p2 = 0.0000008;
    const double k3 = 0.0000009;

    const double skew       = 0.0000010;
    const bool isCalibrated = true;

    const std::string description(UUID::generateUUID());
    const std::string cameraID(UUID::generateUUID());
    const float maxFrameRate                    = 666.66F;
    const data::Camera::PixelFormat pixelFormat = data::Camera::PixelFormat::ARGB8565_PREMULTIPLIED;
    const std::filesystem::path videoFile       = "/tmp/superman fait du vélo.mp4";
    const std::string streamUrl(UUID::generateUUID());
    const data::Camera::SourceType cameraSource = data::Camera::SourceType::UNKNOWN;
    const double scale                          = 0.123456789;

    // Test serialization
    {
        auto camera = data::Camera::New();

        camera->setWidth(width);
        camera->setHeight(height);
        camera->setFx(fx);
        camera->setFy(fy);
        camera->setCx(cx);
        camera->setCy(cy);
        camera->setDistortionCoefficient(k1, k2, p1, p2, k3);
        camera->setSkew(skew);
        camera->setIsCalibrated(isCalibrated);
        camera->setDescription(description);
        camera->setCameraID(cameraID);
        camera->setMaximumFrameRate(maxFrameRate);
        camera->setPixelFormat(pixelFormat);
        camera->setVideoFile(videoFile);
        camera->setStreamUrl(streamUrl);
        camera->setCameraSource(cameraSource);
        camera->setScale(scale);

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(camera);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& camera = data::Camera::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(camera);

        CPPUNIT_ASSERT_EQUAL(width, camera->getWidth());
        CPPUNIT_ASSERT_EQUAL(height, camera->getHeight());

        CPPUNIT_ASSERT_DOUBLES_EQUAL(fx, camera->getFx(), DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(fy, camera->getFy(), DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(cx, camera->getCx(), DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(cy, camera->getCy(), DOUBLE_EPSILON);

        const auto& distortionCoefficient = camera->getDistortionCoefficient();
        CPPUNIT_ASSERT_DOUBLES_EQUAL(k1, distortionCoefficient[0], DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(k2, distortionCoefficient[1], DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(p1, distortionCoefficient[2], DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(p2, distortionCoefficient[3], DOUBLE_EPSILON);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(k3, distortionCoefficient[4], DOUBLE_EPSILON);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(skew, camera->getSkew(), DOUBLE_EPSILON);
        CPPUNIT_ASSERT_EQUAL(isCalibrated, camera->getIsCalibrated());
        CPPUNIT_ASSERT_EQUAL(description, camera->getDescription());
        CPPUNIT_ASSERT_EQUAL(cameraID, camera->getCameraID());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(maxFrameRate, camera->getMaximumFrameRate(), FLOAT_EPSILON);
        CPPUNIT_ASSERT_EQUAL(pixelFormat, camera->getPixelFormat());
        CPPUNIT_ASSERT_EQUAL(videoFile, camera->getVideoFile());
        CPPUNIT_ASSERT_EQUAL(streamUrl, camera->getStreamUrl());
        CPPUNIT_ASSERT_EQUAL(cameraSource, camera->getCameraSource());
        CPPUNIT_ASSERT_DOUBLES_EQUAL(scale, camera->getScale(), DOUBLE_EPSILON);
    }
}

//------------------------------------------------------------------------------

void SessionTest::cameraSeriesTest()
{
    // Create a temporary directory
    const std::filesystem::path tmpfolder = core::tools::System::getTemporaryFolder();
    std::filesystem::create_directories(tmpfolder);
    const std::filesystem::path testPath = tmpfolder / "cameraSeriesTest.zip";

    // Test data
    const std::array<std::string, 3> cameraIDs = {
        UUID::generateUUID(),
        UUID::generateUUID(),
        UUID::generateUUID()
    };

    // Test serialization
    {
        auto cameraSeries = data::CameraSeries::New();

        for(const auto& cameraID : cameraIDs)
        {
            const auto& camera = data::Camera::New();
            camera->setCameraID(cameraID);
            cameraSeries->addCamera(camera);
        }

        // Create the session writer
        auto sessionWriter = io::session::SessionWriter::New();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(cameraSeries);
        sessionWriter->setFile(testPath);
        sessionWriter->write();

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = io::session::SessionReader::New();
        CPPUNIT_ASSERT(sessionReader);
        sessionReader->setFile(testPath);
        sessionReader->read();

        // Test value
        const auto& cameraSeries = data::CameraSeries::dynamicCast(sessionReader->getObject());
        CPPUNIT_ASSERT(cameraSeries);

        for(size_t i = 0, end = cameraSeries->getNumberOfCameras() ; i < end ; ++i)
        {
            const auto& camera = cameraSeries->getCamera(i);
            CPPUNIT_ASSERT_EQUAL(cameraIDs[i], camera->getCameraID());
        }
    }
}

} // namespace ut

} // namespace sight::io::session

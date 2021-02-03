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

#include "PatchTest.hpp"

#include <core/include/core/thread/Worker.hpp>

#include <fwMDSemanticPatch/PatchLoader.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/generator/Image.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/System.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::patchMedicalData::ut::PatchTest );

namespace patchMedicalData
{
namespace ut
{

//------------------------------------------------------------------------------

void PatchTest::setUp()
{
    // Set up context before running a test.
    ::fwServices::registry::ActiveWorkers::setDefaultWorker( core::thread::Worker::New() );
}

//------------------------------------------------------------------------------

void PatchTest::tearDown()
{
    // Clean up after the test run.
    ::fwServices::registry::ActiveWorkers::getDefault()->clearRegistry();
}

//------------------------------------------------------------------------------

template <typename T>
SPTR(T) read(const ::fwRuntime::EConfigurationElement::sptr& srvCfg, const std::string& reader)
{

    typename T::sptr readObj = T::New();
    ::fwServices::IService::sptr readerSrv = ::fwServices::add( reader );
    CPPUNIT_ASSERT(readerSrv);

    readerSrv->registerInOut(readObj, "data");
    readerSrv->setConfiguration(srvCfg);
    readerSrv->configure();
    readerSrv->start().wait();
    readerSrv->update().wait();
    readerSrv->stop().wait();
    ::fwServices::OSR::unregisterService( readerSrv );

    return readObj;
}

//------------------------------------------------------------------------------

::fwMedData::Series::sptr getJMSSeries( const ::fwMedData::SeriesDB::sptr& sdb )
{
    for( ::fwMedData::Series::sptr series :  sdb->getContainer() )
    {
        if ( series->getPatient()->getName() == "SAVA JEAN-MICHEL" )
        {
            return series;
        }
    }

    return ::fwMedData::Series::sptr();
}

//------------------------------------------------------------------------------

std::vector< ::fwMedData::Series::sptr > getOtherSeries( const ::fwMedData::SeriesDB::sptr& sdb )
{
    std::vector< ::fwMedData::Series::sptr > otherSeries;
    for( ::fwMedData::Series::sptr series :  sdb->getContainer() )
    {
        if ( series->getPatient()->getName() != "SAVA JEAN-MICHEL" )
        {
            otherSeries.push_back( series );
        }
    }

    return otherSeries;
}

//------------------------------------------------------------------------------

void PatchTest::patchMedicalDataTest()
{
    const std::filesystem::path file = ::fwTest::Data::dir() /"sight/patch/md_1.jsonz";

    CPPUNIT_ASSERT_MESSAGE("The file '" + file.string() + "' does not exist",
                           std::filesystem::exists(file));

    ::fwRuntime::EConfigurationElement::sptr srvCfg = ::fwRuntime::EConfigurationElement::New("service");

    ::fwRuntime::EConfigurationElement::sptr fileCfg = ::fwRuntime::EConfigurationElement::New("file");
    fileCfg->setValue(file.string());
    srvCfg->addConfigurationElement(fileCfg);

    //<patcher context="..." version="..." />
    ::fwRuntime::EConfigurationElement::sptr patcherCfg = ::fwRuntime::EConfigurationElement::New("patcher");
    patcherCfg->setAttributeValue("context", "MedicalData");
    patcherCfg->setAttributeValue("version", ::fwMDSemanticPatch::PatchLoader::getCurrentVersion());
    srvCfg->addConfigurationElement(patcherCfg);

    ::fwMedData::SeriesDB::sptr sdb = read< ::fwMedData::SeriesDB >(srvCfg, "::ioAtoms::SReader" );

    // check data
    CPPUNIT_ASSERT_EQUAL( (size_t) 3, sdb->size() );

    // Check JMS Data
    ::fwMedData::Series::sptr series = getJMSSeries( sdb );
    CPPUNIT_ASSERT( series );
    CPPUNIT_ASSERT( ::fwMedData::ImageSeries::dynamicCast(series) );
    CPPUNIT_ASSERT_EQUAL(std::string("1.2.392.200036.9116.2.6.1.48.1211418863.1225184516.765855"),
                         series->getInstanceUID());
    CPPUNIT_ASSERT_EQUAL(std::string("20171028"), series->getDate());
    CPPUNIT_ASSERT_EQUAL(std::string("174446"), series->getTime());
    CPPUNIT_ASSERT_EQUAL(std::string("Original image"), series->getDescription());
    CPPUNIT_ASSERT_EQUAL(std::string("CT"), series->getModality());

    ::fwMedData::Patient::sptr patient = series->getPatient();
    CPPUNIT_ASSERT( patient );
    CPPUNIT_ASSERT_EQUAL(std::string("12592 ARTHRO GENOU  G"), patient->getPatientId());
    CPPUNIT_ASSERT_EQUAL(std::string("19970926"), patient->getBirthdate());
    CPPUNIT_ASSERT_EQUAL(std::string("M"), patient->getSex());

    ::fwMedData::Study::sptr study = series->getStudy();
    CPPUNIT_ASSERT( study );
    CPPUNIT_ASSERT_EQUAL(std::string("1.2.392.200036.9116.2.6.1.48.1211418863.1225183167.375775"),
                         study->getInstanceUID());
    CPPUNIT_ASSERT_EQUAL(std::string("20171028"), study->getDate());
    CPPUNIT_ASSERT_EQUAL(std::string("174446"), study->getTime());
    CPPUNIT_ASSERT_EQUAL(std::string(""), study->getReferringPhysicianName());
    CPPUNIT_ASSERT_EQUAL(std::string(""), study->getDescription());
    CPPUNIT_ASSERT_EQUAL(std::string(""), study->getPatientAge());

    ::fwMedData::Equipment::sptr equipment = series->getEquipment();
    CPPUNIT_ASSERT( equipment );
    CPPUNIT_ASSERT_EQUAL(std::string("SCANNER DU MORDOR"), equipment->getInstitutionName());

    // Test split between meshes and image
    std::vector< ::fwMedData::Series::sptr > otherSeries = getOtherSeries( sdb );
    CPPUNIT_ASSERT_EQUAL( (size_t) 2, otherSeries.size() );
    CPPUNIT_ASSERT( otherSeries[0]->getStudy() != otherSeries[1]->getStudy() );
    CPPUNIT_ASSERT( otherSeries[0]->getPatient() != otherSeries[1]->getPatient() );
    CPPUNIT_ASSERT( otherSeries[0]->getEquipment() != otherSeries[1]->getEquipment() );

    ::fwMedData::Patient::sptr p1 = otherSeries[0]->getPatient();
    ::fwMedData::Patient::sptr p2 = otherSeries[1]->getPatient();
    CPPUNIT_ASSERT_EQUAL( p1->getName(), p2->getName() );
    CPPUNIT_ASSERT_EQUAL( p1->getPatientId(), p2->getPatientId() );
    CPPUNIT_ASSERT_EQUAL( p1->getBirthdate(), p2->getBirthdate() );
    CPPUNIT_ASSERT_EQUAL( p1->getSex(), p2->getSex() );

    ::fwMedData::Study::sptr s1 = otherSeries[0]->getStudy();
    ::fwMedData::Study::sptr s2 = otherSeries[1]->getStudy();
    CPPUNIT_ASSERT_EQUAL( s1->getInstanceUID(), s2->getInstanceUID() );
    CPPUNIT_ASSERT_EQUAL( s1->getDate(), s2->getDate() );
    CPPUNIT_ASSERT_EQUAL( s1->getTime(), s2->getTime() );
    CPPUNIT_ASSERT_EQUAL( s1->getDescription(), s2->getDescription() );
    CPPUNIT_ASSERT_EQUAL( s1->getReferringPhysicianName(), s2->getReferringPhysicianName() );
    CPPUNIT_ASSERT_EQUAL( s1->getPatientAge(), s2->getPatientAge() );

    ::fwMedData::Equipment::sptr e1 = otherSeries[0]->getEquipment();
    ::fwMedData::Equipment::sptr e2 = otherSeries[1]->getEquipment();
    CPPUNIT_ASSERT_EQUAL( e1->getInstitutionName(), e2->getInstitutionName() );

    ::fwMedData::Series::sptr se1 = otherSeries[0];
    ::fwMedData::Series::sptr se2 = otherSeries[1];
    CPPUNIT_ASSERT( se1->getInstanceUID() != se2->getInstanceUID() );
    CPPUNIT_ASSERT( se1->getModality() != se2->getModality() );
    CPPUNIT_ASSERT_EQUAL( se1->getDate(), se2->getDate() );
    CPPUNIT_ASSERT_EQUAL( se1->getTime(), se2->getTime() );
    CPPUNIT_ASSERT_EQUAL( se1->getDescription(), se2->getDescription() );
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace patchMedicalData

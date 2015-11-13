/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "SOPClassUIDSplitterTest.hpp"

#include <fwDicomIOFilter/IFilter.hpp>
#include <fwDicomIOFilter/factory/new.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>
#include <fwTest/Data.hpp>
#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <boost/filesystem/path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomIOFilter::ut::SOPClassUIDSplitterTest );

namespace fwDicomIOFilter
{
namespace ut
{

//------------------------------------------------------------------------------

void SOPClassUIDSplitterTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SOPClassUIDSplitterTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void SOPClassUIDSplitterTest::simpleApplication()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename           = "71-CT-DICOM_SEG";
    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/DicomDB" / filename;

    // Read DicomSeries
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
    CPPUNIT_ASSERT_EQUAL(size_t(1), seriesDB->size());

    // Retrieve DicomSeries
    ::fwDicomData::DicomSeries::sptr dicomSeries = ::fwDicomData::DicomSeries::dynamicCast((*seriesDB)[0]);
    CPPUNIT_ASSERT(dicomSeries);
    std::vector< ::fwDicomData::DicomSeries::sptr > dicomSeriesContainer;
    dicomSeriesContainer.push_back(dicomSeries);

    // Apply filter
    ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(
        "::fwDicomIOFilter::splitter::SOPClassUIDSplitter");
    CPPUNIT_ASSERT(filter);
    ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainer, filter, true);
    CPPUNIT_ASSERT_EQUAL(size_t(2), dicomSeriesContainer.size());
    ::fwDicomData::DicomSeries::sptr dicomSeriesA = dicomSeriesContainer[0];
    ::fwDicomData::DicomSeries::sptr dicomSeriesB = dicomSeriesContainer[1];

    // Check SOP Class UIDs
    CPPUNIT_ASSERT_EQUAL(std::string("1.2.840.10008.5.1.4.1.1.2"), *dicomSeriesA->getSOPClassUIDs().begin());       // CT Image Storage
    CPPUNIT_ASSERT_EQUAL(std::string("1.2.840.10008.5.1.4.1.1.66.5"), *dicomSeriesB->getSOPClassUIDs().begin());    // Surface Segmentation Storage

}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwDicomIOFilter

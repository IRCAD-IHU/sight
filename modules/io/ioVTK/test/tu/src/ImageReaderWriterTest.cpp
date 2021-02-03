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

#include "ImageReaderWriterTest.hpp"

#include <fwData/Image.hpp>
#include <fwData/reflection/visitor/CompareObjects.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/generator/Image.hpp>
#include <fwTest/helper/compare.hpp>

#include <fwThread/Worker.hpp>

#include <fwTools/System.hpp>

#include <filesystem>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ioVTK::ut::ImageReaderWriterTest );

static const double epsilon = 0.00001;

namespace ioVTK
{
namespace ut
{

//------------------------------------------------------------------------------

void runImageSrv(
    const std::string& srvname,
    const SPTR(::fwRuntime::EConfigurationElement)& cfg,
    const SPTR(::fwData::Object)& image)
{

    ::fwServices::IService::sptr srv = ::fwServices::add( srvname );

    CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + srvname, srv);

    if (srv->isA("::fwIO::IReader"))
    {
        srv->registerInOut(image, "data");
    }
    else
    {
        srv->registerInput(image, "data");
    }

    CPPUNIT_ASSERT_NO_THROW( srv->setConfiguration(cfg) );
    CPPUNIT_ASSERT_NO_THROW( srv->configure() );
    CPPUNIT_ASSERT_NO_THROW( srv->start().wait() );
    CPPUNIT_ASSERT_NO_THROW( srv->update().wait() );
    CPPUNIT_ASSERT_NO_THROW( srv->stop().wait() );
    ::fwServices::OSR::unregisterService( srv );
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::setUp()
{
    // Set up context before running a test.
    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();
    ::fwServices::registry::ActiveWorkers::setDefaultWorker(worker);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::tearDown()
{
    // Clean up after the test run.
    ::fwServices::registry::ActiveWorkers::getDefault()->clearRegistry();
}

//------------------------------------------------------------------------------

::fwRuntime::EConfigurationElement::sptr getIOConfiguration(const std::filesystem::path& file)
{
    ::fwRuntime::EConfigurationElement::sptr readerSrvCfg = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr readerCfg    = ::fwRuntime::EConfigurationElement::New("file");
    readerCfg->setValue(file.string());
    readerSrvCfg->addConfigurationElement(readerCfg);

    return readerSrvCfg;
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testVtkImageReader()
{

    const std::filesystem::path file = ::fwTest::Data::dir() / "sight/image/vtk/img.vtk";

    CPPUNIT_ASSERT_MESSAGE("The file '" + file.string() + "' does not exist",
                           std::filesystem::exists(file));

    ::fwData::Image::sptr image = ::fwData::Image::New();

    // Data expected
    ::fwData::Image::Spacing spacingExpected;
    spacingExpected[0] = 1.732;
    spacingExpected[1] = 1.732;
    spacingExpected[2] = 3.2;

    ::fwData::Image::Origin originExpected;
    originExpected[0] = 34.64;
    originExpected[1] = 86.6;
    originExpected[2] = 56;

    ::fwData::Image::Size sizeExpected;
    sizeExpected[0] = 230;
    sizeExpected[1] = 170;
    sizeExpected[2] = 58;

    runImageSrv("::ioVTK::SImageReader", getIOConfiguration(file), image);

    // Data read.
    ::fwData::Image::Spacing spacingRead = image->getSpacing2();
    ::fwData::Image::Origin originRead   = image->getOrigin2();
    ::fwData::Image::Size sizeRead       = image->getSize2();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2]);

}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testVtiImageReader()
{
    const std::filesystem::path file = ::fwTest::Data::dir() /"sight/image/vti/BostonTeapot.vti";

    CPPUNIT_ASSERT_MESSAGE("The file '" + file.string() + "' does not exist",
                           std::filesystem::exists(file));

    ::fwData::Image::sptr image = ::fwData::Image::New();
    runImageSrv("::ioVTK::SImageReader", getIOConfiguration(file), image);

    // Data expected
    ::fwData::Image::Spacing spacingExpected;
    spacingExpected[0] = 1.0;
    spacingExpected[1] = 1.0;
    spacingExpected[2] = 1.0;

    ::fwData::Image::Origin originExpected;
    originExpected[0] = 1.1;
    originExpected[1] = 2.2;
    originExpected[2] = 3.3;

    ::fwData::Image::Size sizeExpected;
    sizeExpected[0] = 256;
    sizeExpected[1] = 256;
    sizeExpected[2] = 178;

    ::fwTools::Type expectedType("int8"); // MHD File image type : MET_CHAR

    // Data read.
    ::fwData::Image::Spacing spacingRead = image->getSpacing2();
    ::fwData::Image::Origin originRead   = image->getOrigin2();
    ::fwData::Image::Size sizeRead       = image->getSize2();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2]);

    CPPUNIT_ASSERT_EQUAL( expectedType, image->getType());
}

//------------------------------------------------------------------------------
void ImageReaderWriterTest::testMhdImageReader()
{

    const std::filesystem::path file = ::fwTest::Data::dir() / "sight/image/mhd/BostonTeapot.mhd";

    CPPUNIT_ASSERT_MESSAGE("The file '" + file.string() + "' does not exist",
                           std::filesystem::exists(file));

    ::fwData::Image::sptr image = ::fwData::Image::New();
    runImageSrv("::ioVTK::SImageReader", getIOConfiguration(file), image);

    // Data expected
    ::fwData::Image::Spacing spacingExpected;
    spacingExpected[0] = 1.0;
    spacingExpected[1] = 1.0;
    spacingExpected[2] = 1.0;

    ::fwData::Image::Origin originExpected;
    originExpected[0] = 1.1;
    originExpected[1] = 2.2;
    originExpected[2] = 3.3;

    ::fwData::Image::Size sizeExpected;
    sizeExpected[0] = 256;
    sizeExpected[1] = 256;
    sizeExpected[2] = 178;

    ::fwTools::Type expectedType("int8"); // MHD File image type : MET_CHAR

    // Data read.
    ::fwData::Image::Spacing spacingRead = image->getSpacing2();
    ::fwData::Image::Origin originRead   = image->getOrigin2();
    ::fwData::Image::Size sizeRead       = image->getSize2();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2]);

    CPPUNIT_ASSERT_EQUAL( expectedType, image->getType());

}
//------------------------------------------------------------------------------

void ImageReaderWriterTest::testImageReaderExtension()
{
    const std::filesystem::path file = ::fwTools::System::getTemporaryFolder() / "img.xxx";

    std::ofstream ofile;
    ofile.open(file.string().c_str());
    ofile.close();

    ::fwData::Image::sptr image = ::fwData::Image::New();

    {
        const std::string srvname("::ioVTK::SImageReader");

        ::fwServices::IService::sptr srv = ::fwServices::add( srvname );

        CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + srvname, srv);

        srv->registerInOut(image, "data");

        CPPUNIT_ASSERT_NO_THROW( srv->setConfiguration(getIOConfiguration(file)) );
        CPPUNIT_ASSERT_NO_THROW( srv->configure() );
        CPPUNIT_ASSERT_NO_THROW( srv->start().wait() );
        CPPUNIT_ASSERT_THROW( srv->update().get(), ::fwTools::Failed);
        CPPUNIT_ASSERT_NO_THROW( srv->stop().wait() );
        ::fwServices::OSR::unregisterService( srv );
    }
    std::filesystem::remove(file);

}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testBitmapImageWriter()
{
    // Data to write
    const ::fwTools::Type type               = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::Size sizeExpected = {10, 20, 0};
    // Use standard information for spacing and origin
    // As the data will be lost in the file format
    const ::fwData::Image::Spacing spacingExpected = {1., 1., 0};
    const ::fwData::Image::Origin originExpected   = {0., 0., 0.};

    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateImage(image, sizeExpected, spacingExpected, originExpected, type,
                                              ::fwData::Image::RGB);

    // Test all teh available extensions
    std::vector<std::string> extensions;
    extensions.push_back("bmp");
    extensions.push_back("jpeg");
    extensions.push_back("jpg");
    extensions.push_back("png");
    extensions.push_back("pnm");
    extensions.push_back("tiff");

    for(const std::string ext : extensions)
    {
        // Write to bitmap image.
        const std::filesystem::path file = ::fwTools::System::getTemporaryFolder() / ("temporaryFile." + ext);

        runImageSrv("::ioVTK::SImageWriter", getIOConfiguration(file), image);

        // Read image from disk
        const ::fwData::Image::sptr imageFromDisk = ::fwData::Image::New();
        runImageSrv("::ioVTK::SImageReader", getIOConfiguration(file), imageFromDisk);

        std::filesystem::remove(file);

        // Data read
        ::fwData::Image::Spacing spacingRead = image->getSpacing2();
        ::fwData::Image::Origin originRead   = image->getOrigin2();
        ::fwData::Image::Size sizeRead       = image->getSize2();

        CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
        CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
        CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);

        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);

        CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);

        const auto imageDumpLock         = image->lock();
        const auto imageFromDiskDumpLock = imageFromDisk->lock();

        const char* const ptrOnGeneratedImage = static_cast<char*>(image->getBuffer());
        const char* const ptrOnReadImage      = static_cast<char*>(imageFromDisk->getBuffer());

        CPPUNIT_ASSERT_EQUAL( image->getType(), imageFromDisk->getType() );
        CPPUNIT_ASSERT( std::equal(ptrOnGeneratedImage, ptrOnGeneratedImage + image->getSizeInBytes(),
                                   ptrOnReadImage) );
    }
}

//------------------------------------------------------------------------------
void ImageReaderWriterTest::testVtkImageWriter()
{
    // Data to write
    ::fwTools::Type type = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::Size sizeExpected       = {10, 20, 30};
    const ::fwData::Image::Spacing spacingExpected = {0.24, 1.07, 2.21};
    const ::fwData::Image::Origin originExpected   = {-5.6, 15.16, 11.11};

    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateImage(image, sizeExpected, spacingExpected, originExpected, type,
                                              ::fwData::Image::RGBA);
    ::fwTest::generator::Image::randomizeImage(image);

    // Write to vtk image.
    const std::filesystem::path file = ::fwTools::System::getTemporaryFolder() / "temporaryFile.vtk";

    runImageSrv("::ioVTK::SImageWriter", getIOConfiguration(file), image);

    // Read image from disk
    ::fwData::Image::sptr imageFromDisk = ::fwData::Image::New();
    runImageSrv("::ioVTK::SImageReader", getIOConfiguration(file), imageFromDisk);

    std::filesystem::remove(file);

    // Data read
    ::fwData::Image::Spacing spacingRead = image->getSpacing2();
    ::fwData::Image::Origin originRead   = image->getOrigin2();
    ::fwData::Image::Size sizeRead       = image->getSize2();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2]);

    const auto imageDumpLock         = image->lock();
    const auto imageFromDiskDumpLock = imageFromDisk->lock();

    const char* const ptrOnGeneratedImage = static_cast<char*>(image->getBuffer());
    const char* const ptrOnReadImage      = static_cast<char*>(imageFromDisk->getBuffer());

    CPPUNIT_ASSERT_EQUAL( image->getType(), imageFromDisk->getType() );
    CPPUNIT_ASSERT( std::equal(ptrOnGeneratedImage, ptrOnGeneratedImage + image->getSizeInBytes(), ptrOnReadImage) );

}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testVtkImageSeriesWriter()
{
    ::fwTools::Type type        = ::fwTools::Type::s_FLOAT;
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(image, type);

    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::New();
    imageSeries->setImage(image);

    const std::filesystem::path file = ::fwTools::System::getTemporaryFolder() / "imageSeries.vtk";

    // Write image series
    runImageSrv("::ioVTK::SImageSeriesWriter", getIOConfiguration(file), imageSeries);

    // Read image series
    ::fwData::Image::sptr newImage = ::fwData::Image::New();
    runImageSrv("::ioVTK::SImageReader", getIOConfiguration(file), newImage);

    ::fwTest::helper::ExcludeSetType exclude;
    exclude.insert("window_center");
    exclude.insert("window_width");

    CPPUNIT_ASSERT(::fwTest::helper::compare(image, newImage, exclude));
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testVtiImageWriter()
{
    // Data to write
    ::fwTools::Type type = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::Size sizeExpected       = {10, 20, 30};
    const ::fwData::Image::Spacing spacingExpected = {0.24, 1.07, 2.21};
    const ::fwData::Image::Origin originExpected   = {-5.6, 15.16, 11.11};

    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateImage(image, sizeExpected, spacingExpected, originExpected, type,
                                              ::fwData::Image::GRAY_SCALE);
    ::fwTest::generator::Image::randomizeImage(image);

    // Write to vtk image.
    const std::filesystem::path file = ::fwTools::System::getTemporaryFolder() / "temporaryFile.vti";

    runImageSrv("::ioVTK::SImageWriter", getIOConfiguration(file), image);

    // Read image from disk
    ::fwData::Image::sptr imageFromDisk = ::fwData::Image::New();
    runImageSrv("::ioVTK::SImageReader", getIOConfiguration(file), imageFromDisk);

    // Data read
    ::fwData::Image::Spacing spacingRead = image->getSpacing2();
    ::fwData::Image::Origin originRead   = image->getOrigin2();
    ::fwData::Image::Size sizeRead       = image->getSize2();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2]);

    const auto imageDumpLock         = image->lock();
    const auto imageFromDiskDumpLock = imageFromDisk->lock();

    const char* const ptrOnGeneratedImage = static_cast<char*>(image->getBuffer());
    const char* const ptrOnReadImage      = static_cast<char*>(imageFromDisk->getBuffer());

    CPPUNIT_ASSERT_EQUAL( image->getType(), imageFromDisk->getType());
    CPPUNIT_ASSERT( std::equal(ptrOnGeneratedImage, ptrOnGeneratedImage + image->getSizeInBytes(), ptrOnReadImage) );

}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testMhdImageWriter()
{
    // Data to write
    ::fwTools::Type type = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::Size sizeExpected       = {10, 20, 30};
    const ::fwData::Image::Spacing spacingExpected = {0.24, 1.07, 2.21};
    const ::fwData::Image::Origin originExpected   = {-5.6, 15.16, 11.11};

    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateImage(image, sizeExpected, spacingExpected, originExpected, type,
                                              ::fwData::Image::RGB);
    ::fwTest::generator::Image::randomizeImage(image);

    // Write to vtk image.
    const std::filesystem::path file = ::fwTools::System::getTemporaryFolder()/ "temporaryFile.mhd";

    runImageSrv("::ioVTK::SImageWriter", getIOConfiguration(file), image);

    // Read image from disk
    ::fwData::Image::sptr imageFromDisk = ::fwData::Image::New();
    runImageSrv("::ioVTK::SImageReader", getIOConfiguration(file), imageFromDisk);

    // Data read
    ::fwData::Image::Spacing spacingRead = image->getSpacing2();
    ::fwData::Image::Origin originRead   = image->getOrigin2();
    ::fwData::Image::Size sizeRead       = image->getSize2();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2]);

    const auto imageDumpLock         = image->lock();
    const auto imageFromDiskDumpLock = imageFromDisk->lock();

    const char* const ptrOnGeneratedImage = static_cast<char*>(image->getBuffer());
    const char* const ptrOnReadImage      = static_cast<char*>(imageFromDisk->getBuffer());

    CPPUNIT_ASSERT_EQUAL( image->getType(), imageFromDisk->getType());
    CPPUNIT_ASSERT( std::equal(ptrOnGeneratedImage, ptrOnGeneratedImage + image->getSizeInBytes(), ptrOnReadImage) );

}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testImageWriterExtension()
{
    // Data to write
    ::fwTools::Type type = ::fwTools::Type::s_UINT8;
    const ::fwData::Image::Size sizeExpected       = {10, 20, 30};
    const ::fwData::Image::Spacing spacingExpected = {0.24, 1.07, 2.21};
    const ::fwData::Image::Origin originExpected   = {-5.6, 15.16, 11.11};

    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateImage(image, sizeExpected, spacingExpected, originExpected, type,
                                              ::fwData::Image::GRAY_SCALE);
    ::fwTest::generator::Image::randomizeImage(image);

    // Write to vtk image.
    const std::filesystem::path file = ::fwTools::System::getTemporaryFolder()/ "temporaryFile.xxx";

    {
        const std::string srvname("::ioVTK::SImageWriter");

        ::fwServices::IService::sptr srv = ::fwServices::add( srvname );

        CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + srvname, srv);

        srv->registerInput(image, "data");
        CPPUNIT_ASSERT_NO_THROW( srv->setConfiguration(getIOConfiguration(file)) );
        CPPUNIT_ASSERT_NO_THROW( srv->configure() );
        CPPUNIT_ASSERT_NO_THROW( srv->start().wait() );
        CPPUNIT_ASSERT_THROW( srv->update().get(), ::fwTools::Failed);
        CPPUNIT_ASSERT_NO_THROW( srv->stop().wait() );
        ::fwServices::OSR::unregisterService( srv );
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace ioVTK

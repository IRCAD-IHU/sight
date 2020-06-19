/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "ImageTest.hpp"

#include <fwTest/generator/Image.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwTools/Type.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTest::ut::ImageTest );

namespace fwTest
{
namespace ut
{

//-----------------------------------------------------------------------------

void compare(::fwData::Object::sptr objRef, ::fwData::Object::sptr objComp)
{
    ::fwDataCamp::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    for( ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop :  (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    CPPUNIT_ASSERT_MESSAGE("Object Not equal", props->size() == 0 );
}

//------------------------------------------------------------------------------

void ImageTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
}

//------------------------------------------------------------------------------

void ImageTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageTest::generator3DTest()
{
    ::fwData::Image::sptr image = ::fwData::Image::New();
    const ::fwData::Image::Size size       = {10, 15, 23};
    const ::fwData::Image::Spacing spacing = {0.85, 2.6, 1.87};
    const ::fwData::Image::Origin origin   = {-45.6, 25.97, -53.9};
    ::fwTools::Type type = ::fwTools::Type::create< std::int32_t >();

    ::fwTest::generator::Image::generateImage(image, size, spacing, origin, type,
                                              ::fwData::Image::PixelFormat::GRAY_SCALE);

    CPPUNIT_ASSERT(image->getSize2() == size);
    CPPUNIT_ASSERT(image->getSpacing2() == spacing);
    CPPUNIT_ASSERT(image->getOrigin2() == origin);
    CPPUNIT_ASSERT_EQUAL(type.string(), image->getType().string());
}

//------------------------------------------------------------------------------

void ImageTest::generator2DTest()
{
    ::fwData::Image::sptr image = ::fwData::Image::New();
    const ::fwData::Image::Size size       = {10, 15, 0};
    const ::fwData::Image::Spacing spacing = {0.85, 2.6, 0.};
    const ::fwData::Image::Origin origin   = {-45.6, 25.97, 0.};
    ::fwTools::Type type = ::fwTools::Type::create< std::int16_t >();

    ::fwTest::generator::Image::generateImage(image, size, spacing, origin, type,
                                              ::fwData::Image::PixelFormat::GRAY_SCALE);
    CPPUNIT_ASSERT(image->getSize2() == size);
    CPPUNIT_ASSERT(image->getSpacing2() == spacing);
    CPPUNIT_ASSERT(image->getOrigin2() == origin);
    CPPUNIT_ASSERT_EQUAL(type.string(), image->getType().string());
}

//------------------------------------------------------------------------------

void ImageTest::deepCopyTest()
{
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwData::Image::sptr image2;
    ::fwTools::Type type = ::fwTools::Type::create< std::int32_t >();
    ::fwTest::generator::Image::generateRandomImage(image, type);

    image2 = ::fwData::Object::copy(image);
    compare(image, image2);
}

//------------------------------------------------------------------------------

void ImageTest::stressTest()
{
    ::fwTools::Type type = ::fwTools::Type::create< std::int8_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::uint8_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::int16_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::uint16_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::int32_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::uint32_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::int64_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< std::uint64_t >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< float >();
    this->stressTestWithType(type, 10);

    type = ::fwTools::Type::create< double >();
    this->stressTestWithType(type, 10);
}

//------------------------------------------------------------------------------

void ImageTest::stressTestWithType(::fwTools::Type type, int nbTest)
{
    for (int nb = 0; nb < nbTest; ++nb)
    {
        ::fwData::Image::sptr image = ::fwData::Image::New();
        ::fwTest::generator::Image::generateRandomImage(image, type);

        ::fwData::Image::sptr image2;
        image2 = ::fwData::Object::copy(image);
        compare(image, image2);
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwTest

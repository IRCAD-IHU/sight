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

#include "reflection/GetObjectTest.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Float.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/reflection/exception/NullPointer.hpp>
#include <fwData/reflection/exception/ObjectNotFound.hpp>
#include <fwData/reflection/getObject.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwTest/generator/Image.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::reflection::ut::GetObjectTest );

namespace fwData
{
namespace reflection
{
namespace ut
{

//-----------------------------------------------------------------------------

// Set up context before running a test.
void GetObjectTest::setUp()
{
}

//-----------------------------------------------------------------------------

void GetObjectTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void GetObjectTest::getTest()
{
    // Visit 1
    ::fwData::Image::sptr img1 = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img1, ::fwTools::Type::create("int16"));
    ::fwData::Image::sptr img2 = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img2, ::fwTools::Type::create("uint8"));
    ::fwData::Composite::sptr composite = ::fwData::Composite::New();
    composite->getContainer()["img1"]   = img1;
    composite->getContainer()["img2"]   = img2;
    ::fwData::Object::sptr subObj1      = ::fwData::reflection::getObject( composite, "@values.img2" );
    CPPUNIT_ASSERT_MESSAGE("Image must be equal", subObj1 == img2);

    // Visit 2
    ::fwData::Float::sptr zspacing = ::fwData::reflection::getObject< ::fwData::Float >( composite,
                                                                                         "@values.img2.spacing.2" );
    CPPUNIT_ASSERT_MESSAGE("spacing must be equal",
                           img2->getSpacing2()[2] - 0.001 < zspacing->value() &&
                           zspacing->value() < img2->getSpacing2()[2] + 0.001 );

    // Visit 3
    composite->setField("myImage1", img1);
    img1->setField("myImage2", img2);
    ::fwData::Object::sptr subObj2 = ::fwData::reflection::getObject( composite, "@fields.myImage1.fields.myImage2" );
    CPPUNIT_ASSERT_MESSAGE("Image must be equal", subObj2 == img2 );
}

//-----------------------------------------------------------------------------

void GetObjectTest::invalidPathTest()
{
    ::fwData::Composite::sptr composite = ::fwData::Composite::New();
    ::fwData::String::sptr text         = ::fwData::String::New("Text");
    (*composite)["string"]              = text;
    ::fwData::Integer::sptr intValue    = ::fwData::Integer::New(321);
    ::fwData::Float::sptr floatValue    = ::fwData::Float::New(1.234f);
    ::fwData::Vector::sptr vector       = ::fwData::Vector::New();
    ::fwData::Image::sptr img           = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img, ::fwTools::Type::create("int16"));

    auto& internalVector = vector->getContainer();
    internalVector.push_back(intValue);
    internalVector.push_back(floatValue);
    internalVector.push_back(img);
    (*composite)["vector"] = vector;

    // no exception version
    ::fwData::Object::sptr obj = ::fwData::reflection::getObject( composite, "@values.string" );
    CPPUNIT_ASSERT_MESSAGE("fwData::String must be equal", obj == text );

    // with exception version
    obj = ::fwData::reflection::getObject( composite, "@values.string", true );
    CPPUNIT_ASSERT_MESSAGE("fwData::String must be equal", obj == text );

    // no exception version
    ::fwData::Object::sptr invalidObj = ::fwData::reflection::getObject( composite, "@values.invalidPath", false );
    CPPUNIT_ASSERT_MESSAGE("Object must not exist", !invalidObj );

    // exception version : path is invalid
    CPPUNIT_ASSERT_THROW(
        ::fwData::reflection::getObject( composite, "@values.invalidPath", true ),
        ::fwData::reflection::exception::ObjectNotFound
        );
    CPPUNIT_ASSERT_EQUAL(size_t(2), composite->size() );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Vector tests

    // no exception version
    obj = ::fwData::reflection::getObject( composite, "@values.vector.values.0" );
    CPPUNIT_ASSERT_MESSAGE("fwData::Integer must be equal", obj == intValue );

    // with exception version
    obj = ::fwData::reflection::getObject( composite, "@values.vector.values.0", true );
    CPPUNIT_ASSERT_MESSAGE("fwData::Integer must be equal", obj == intValue );

    /// This is important to test vectors subobjects properties to ensure the visitor path is correct

    // no exception version
    obj                            = ::fwData::reflection::getObject( composite, "@values.vector.values.2.spacing.2" );
    ::fwData::Float::sptr zspacing = ::std::dynamic_pointer_cast< ::fwData::Float >(obj);
    CPPUNIT_ASSERT_MESSAGE("spacing must be equal",
                           img->getSpacing2()[2] - 0.001 < zspacing->value() &&
                           zspacing->value() < img->getSpacing2()[2] + 0.001 );

    // with exception version
    obj      = ::fwData::reflection::getObject( composite, "@values.vector.values.2.spacing.2", true );
    zspacing = ::std::dynamic_pointer_cast< ::fwData::Float >(obj);
    CPPUNIT_ASSERT_MESSAGE("spacing must be equal",
                           img->getSpacing2()[2] - 0.001 < zspacing->value() &&
                           zspacing->value() < img->getSpacing2()[2] + 0.001 );

    // out of bounds, no exception version
    invalidObj = ::fwData::reflection::getObject( composite, "@values.vector.values.2.spacing.15", false );
    CPPUNIT_ASSERT_MESSAGE("Object must not exist", !invalidObj );

    // out of bounds, with exception version
    CPPUNIT_ASSERT_THROW( ::fwData::reflection::getObject( composite, "@values.vector.values.2.spacing.15", true ),
                          ::fwData::reflection::exception::NullPointer);

}

//-----------------------------------------------------------------------------

} // namespace ut
} // namespace reflection
} // namespace fwData

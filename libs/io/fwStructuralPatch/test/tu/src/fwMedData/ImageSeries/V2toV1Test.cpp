/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "fwMedData/ImageSeries/V2ToV1Test.hpp"

#include <fwStructuralPatch/fwMedData/ImageSeries/V2ToV1.hpp>

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <utest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwMedData::ImageSeries::V2ToV1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwMedData
{
namespace ImageSeries
{

//------------------------------------------------------------------------------

void V2ToV1Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void V2ToV1Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void V2ToV1Test::applyPatchTest()
{
    ::fwAtoms::Object::sptr imgSeriesV2 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr imgSeriesV1;

    ::fwAtomsPatch::helper::setClassname(imgSeriesV2, "::fwMedData::ImageSeries");
    ::fwAtomsPatch::helper::setVersion(imgSeriesV2, "2");

    ::fwAtomsPatch::helper::Object helper(imgSeriesV2);

    helper.addAttribute("image", ::fwAtoms::Object::sptr());

    helper.addAttribute("patient", ::fwAtoms::Object::sptr());
    helper.addAttribute("study", ::fwAtoms::Object::sptr());
    helper.addAttribute("equipment", ::fwAtoms::Object::sptr());
    helper.addAttribute("instance_uid", ::fwAtoms::String::New("UID"));
    helper.addAttribute("modality", ::fwAtoms::String::New("OT"));
    helper.addAttribute("date", ::fwAtoms::String::New("0"));
    helper.addAttribute("time", ::fwAtoms::String::New("0"));
    helper.addAttribute("performing_physicians_name", ::fwAtoms::Sequence::New());
    helper.addAttribute("description", ::fwAtoms::String::New("Description"));

    // Check if attribute "dicom_reference" is removed in ImageSeries V1
    {
        imgSeriesV1 = ::fwAtoms::Object::dynamicCast(imgSeriesV2->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[imgSeriesV2] = imgSeriesV1;

        ::fwStructuralPatch::fwMedData::ImageSeries::V2ToV1::sptr v2ToV1Patch;
        v2ToV1Patch = ::fwStructuralPatch::fwMedData::ImageSeries::V2ToV1::New();
        CPPUNIT_ASSERT_NO_THROW(v2ToV1Patch->apply(imgSeriesV2, imgSeriesV1, newVersions));

        CPPUNIT_ASSERT(imgSeriesV1);
        CPPUNIT_ASSERT_EQUAL(std::string("1"), ::fwAtomsPatch::helper::getVersion(imgSeriesV1));
        const auto attributes = imgSeriesV1->getAttributes();
        CPPUNIT_ASSERT(attributes.find("dicom_reference") == attributes.end());
    }
}

//------------------------------------------------------------------------------
} //namespace ImageSeries
} //namespace fwMedData
} //namespace ut
} //namespace fwStructuralPatch

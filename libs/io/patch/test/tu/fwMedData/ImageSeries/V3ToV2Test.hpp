/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::patch::structural
{

namespace ut
{

namespace data
{

namespace ImageSeries
{

/// Test structural patch to convert a data::ImageSeries from version '3' to version '2'.
class V3ToV2Test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(V3ToV2Test);
CPPUNIT_TEST(applyPatchTest);
CPPUNIT_TEST_SUITE_END();

public:

    /// Sets up context before running a test.
    void setUp();

    /// Cleans up after the test run.
    void tearDown();

    void applyPatchTest();
};

} //namespace ImageSeries

} //namespace data

} //namespace ut

} //namespace sight::io::patch::structural

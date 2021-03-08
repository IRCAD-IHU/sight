/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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
namespace Camera
{

/**
 * @brief Test patch to convert data::Reconstruction from version 1 to version 2.
 */
class V2ToV1Test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( V2ToV1Test );
CPPUNIT_TEST( applyPatchTestFile );
CPPUNIT_TEST( applyPatchTestStream );
CPPUNIT_TEST( applyPatchTestDevice );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void applyPatchTestFile();
    void applyPatchTestStream();
    void applyPatchTestDevice();
};

} //namespace Camera
} //namespace data
} //namespace ut
} //namespace sight::io::patch::structural

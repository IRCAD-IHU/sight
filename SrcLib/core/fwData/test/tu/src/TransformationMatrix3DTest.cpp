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

#include "TransformationMatrix3DTest.hpp"

#include <fwData/TransformationMatrix3D.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::TransformationMatrix3DTest );

namespace fwData
{
namespace ut
{

//------------------------------------------------------------------------------

void TransformationMatrix3DTest::setUp()
{
    // Set up context before running a test.

}

//------------------------------------------------------------------------------

void TransformationMatrix3DTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void TransformationMatrix3DTest::getterSetterByCoef()
{
    double identity[] = { 1., 0., 0., 0.,
                          0., 1., 0., 0.,
                          0., 0., 1., 0.,
                          0., 0., 0., 1.};

    ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();

    // Check default initialization
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat->getCoefficient(i, j), identity[i * 4 + j]);
        }
    }

    // Call setter and check getter
    double coefs[] = { 2, -2, .3, .12,
                       4, 8.9, 4.2, 1.2,
                       7.8, -12.1, 2.3, 1.2,
                       .3, 1.21, -3.1, 1.2};

    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            mat->setCoefficient(i, j, coefs[i * 4 + j]);
        }
    }

    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat->getCoefficient(i, j), coefs[i * 4 + j]);
        }
    }
}

//------------------------------------------------------------------------------

void TransformationMatrix3DTest::getterSetterByArray()
{
    ::fwData::TransformationMatrix3D::TMCoefArray identity = { 1., 0., 0., 0.,
                                                               0., 1., 0., 0.,
                                                               0., 0., 1., 0.,
                                                               0., 0., 0., 1.};

    ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();

    // Check default initialization
    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat->getCoefficient(i, j), identity[i * 4 + j]);
        }
    }

    // Call setter and check getter
    ::fwData::TransformationMatrix3D::TMCoefArray coefs = { 2, -2, .3, .12,
                                                            4, 8.9, 4.2, 1.2,
                                                            7.8, -12.1, 2.3, 1.2,
                                                            .3, 1.21, -3.1, 1.2};

    mat->setCoefficients(coefs);

    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(mat->getCoefficient(i, j), coefs[i * 4 + j]);
        }
    }

    const ::fwData::TransformationMatrix3D::TMCoefArray& coefs2 = mat->getCoefficients();
    for (size_t i = 0; i < 16; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(coefs2[i], coefs[i]);
    }
}

//------------------------------------------------------------------------------

void TransformationMatrix3DTest::getterSetterByMatrix()
{
    ::fwData::TransformationMatrix3D::MatrixType matrix4x4 = { { {0., 0., 1., 100.},
                                                                 {0., 1., 0., 200.},
                                                                 {1., 0., 0., 3000.},
                                                                 {0., 0., 0., 1.} } };

    ::fwData::TransformationMatrix3D::sptr mat = ::fwData::TransformationMatrix3D::New();

    mat->setMatrix4x4(matrix4x4);

    const auto mat2 = mat->getMatrix4x4();

    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(matrix4x4[i][j], mat2[i][j]);
            CPPUNIT_ASSERT_EQUAL(matrix4x4[i][j], mat->getCoefficient(i, j));
        }
    }

    ::fwData::TransformationMatrix3D::TMCoefArray coefs = { 1, -2, .3, .4,
                                                            5.5, 6, 7.77, 8.,
                                                            0.09, 10., -11., 1.2,
                                                            0, 0, 0, 1.};

    mat->setCoefficients(coefs);

    const auto matFromCoefs = mat->getMatrix4x4();

    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            CPPUNIT_ASSERT_EQUAL(coefs[i * 4 + j], matFromCoefs[i][j]);
        }
    }

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwData

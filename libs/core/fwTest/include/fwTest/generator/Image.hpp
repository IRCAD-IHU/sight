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

#pragma once

#include "fwTest/config.hpp"

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwMath/Compare.hpp>

#include <fwTools/Type.hpp>

namespace fwTest
{
namespace generator
{

/**
 * @brief   This class contains helper to generate images.
 */
class Image
{

public:

    /**
     * @brief Initialize 'rand' seed
     */
    FWTEST_API static void initRand();

    /**
     * @brief Generate an image with the given informations. Buffer is filled with 0.
     * @param image     image to generate
     * @param size      vector of image size
     * @param spacing   vector of image spacing
     * @param origin    vector of image origin
     * @param type      image type
     */
    [[deprecated("It will be removed in sight 22.0")]] FWTEST_API static void generateImage(::fwData::Image::sptr image,
                                                                                            ::fwData::Image::SizeType size,
                                                                                            std::vector<double> spacing,
                                                                                            std::vector<double> origin,
                                                                                            ::fwTools::Type type);

    /**
     * @brief Generate an image with the given informations. Buffer is filled with 0.
     * @param image     image to generate
     * @param size      vector of image size
     * @param spacing   vector of image spacing
     * @param origin    vector of image origin
     * @param type      image type
     * @param format    image format
     */
    FWTEST_API static void generateImage(::fwData::Image::sptr image,
                                         fwData::Image::Size size,
                                         fwData::Image::Spacing spacing,
                                         fwData::Image::Origin origin,
                                         ::fwTools::Type type,
                                         fwData::Image::PixelFormat format);

    /// Generate an image with random information (size, spacing, ...). Buffer is filled with random values.
    FWTEST_API static void generateRandomImage(::fwData::Image::sptr image, ::fwTools::Type type);

    /// Fill image array with random value
    FWTEST_API static void randomizeImage(::fwData::Image::sptr image);

    /// Fill array with random value
    FWTEST_API static void randomizeArray(::fwData::Array::sptr array);

    /// Creates an Array with the given type and size and fills buffer with random values.
    FWTEST_API static ::fwData::Array::sptr createRandomizedArray(const std::string& type,
                                                                  ::fwData::Array::SizeType sizes);

    /// Fill array with random value
    template<typename TYPE>
    static void randomizeArray(const ::fwData::Array::sptr& array)
    {
        ::fwDataTools::helper::Array helper(array);
        TYPE* iter = helper.begin< TYPE >();

        for (; iter != helper.end< TYPE >(); ++iter)
        {
            *iter = static_cast<TYPE>(rand()%256);
        }
    }

    //------------------------------------------------------------------------------

    template<typename TYPE>
    static ::fwData::Array::sptr createRandomizedArray(::fwData::Array::SizeType sizes)
    {
        ::fwData::Array::sptr array = ::fwData::Array::New();

        array->resize( ::fwTools::Type::create<TYPE>(), sizes, 1, true );

        Image::randomizeArray<TYPE>( array );

        return array;
    }

};

} // namespace generator
} // namespace fwTest

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

#include "fwDataTools/config.hpp"

#include <fwData/Image.hpp>

namespace fwDataTools
{
namespace helper
{

/**
 * @brief   Defines an helper to modify an ::fwData::Image by adding few medical fields
 * and create in parallel the message to announce these modifications.
 *
 * @deprecated will be removed in sight 22.0, please use ::fwData::Image.
 */
class FWDATATOOLS_CLASS_API Image
{

public:

    typedef ::fwData::Image::SizeType SizeType;
    typedef ::fwData::Image::IndexType IndexType;
    typedef ::fwData::Image::BufferType BufferType;
    typedef ::fwData::Image::BufferIndexType BufferIndexType;

    /// Shared pointer type
    typedef std::shared_ptr < ::fwDataTools::helper::Image > sptr;
    /// Const shared pointer type
    typedef std::shared_ptr < const ::fwDataTools::helper::Image > csptr;
    /// Unique pointer type
    typedef std::unique_ptr < ::fwDataTools::helper::Image > uptr;
    /// Const unique pointer type
    typedef std::unique_ptr < const ::fwDataTools::helper::Image > cuptr;

    //------------------------------------------------------------------------------

    static sptr New(::fwData::Image::sptr image)
    {
        return std::make_shared< ::fwDataTools::helper::Image >(image);
    }

    /// Constructor. Initialize parameters.
    [[deprecated("will be removed in sight 22.0, please use ::fwData::Image")]]
    FWDATATOOLS_API Image( ::fwData::Image::sptr image );

    /// Destrucotr. Do nothing.
    FWDATATOOLS_API virtual ~Image();

    /**
     * @brief Create landmark fields if not exist, NOT GENERATE MESSAGE.
     *
     * If the image hasn't landmark field, it is added and the method return true.
     *
     * @return true if the field is added.
     *
     * @deprecated use::fwDataTools::fieldHelper::MedicalImageHelpers::checkLandmarks() instead.
     */
    [[deprecated("will be removed in sight 22.0, use "
                 "'::fwDataTools::fieldHelper::MedicalImageHelpers::checkLandmarks()' instead.")]]
    FWDATATOOLS_API bool createLandmarks();

    /**
     * @brief Create transfer function pool if the pool is not present, NOT GENERATE MESSAGE.
     *
     * If the image has not transfer functions, the pool is
     * created and a grey level transfer function is added.
     *
     * @deprecated use::fwDataTools::fieldHelper::MedicalImageHelpers::checkTransferFunctionPool() instead.
     */
    [[deprecated("will be removed in sight 22.0, use "
                 "'::fwDataTools::fieldHelper::MedicalImageHelpers::checkTransferFunctionPool()' instead.")]]
    FWDATATOOLS_API bool createTransferFunctionPool();

    /**
     * @brief Create slice index if indexes are not present, NOT GENERATE MESSAGE.
     *
     * If the image hasn't frontal, axial and sagittal slice index fields, they are added.
     *
     * @return true if the fields are added.
     *
     * @deprecated use::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageSliceIndex() instead.
     */
    [[deprecated("will be removed in sight 22.0, use "
                 "'::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageSliceIndex()' instead.")]]
    FWDATATOOLS_API bool createImageSliceIndex();

    /// Send the message of modification
    FWDATATOOLS_API void notify();

    /// Returns image buffer
    FWDATATOOLS_API void* getBuffer();

    /** @{
     * @brief Helpers for 3D images
     */
    FWDATATOOLS_API void* getPixelBuffer( SizeType::value_type x, SizeType::value_type y, SizeType::value_type z );

    FWDATATOOLS_API void* getPixelBuffer( IndexType index );

    FWDATATOOLS_API void setPixelBuffer( IndexType index, Image::BufferType* pixBuf);

    FWDATATOOLS_API const std::string getPixelAsString(SizeType::value_type x,
                                                       SizeType::value_type y,
                                                       SizeType::value_type z );
    // @}

    /// Returns a copy of current lock on image
    FWDATATOOLS_API ::fwMemory::BufferObject::Lock getLock() const;

private:

    ::fwData::Image::sptr m_image;

    ::fwMemory::BufferObject::Lock m_lock;

    /// modification flags
    bool m_sliceModified;
};

} // helper
} // fwDataTools

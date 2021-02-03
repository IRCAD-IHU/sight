/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGdcmIO/exception/Failed.hpp"
#include "fwGdcmIO/writer/ie/InformationEntity.hpp"

#include <data/Image.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @brief Spatial Fiducials Information Entity class
 */
class FWGDCMIO_CLASS_API SpatialFiducials : public ::fwGdcmIO::writer::ie::InformationEntity< data::Image >
{

public:
    /**
     * @brief Constructor
     * @param[in] writer GDCM writer that must be enriched
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] series Series data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API SpatialFiducials(const SPTR(::gdcm::Writer)& writer,
                                  const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                  const data::Image::csptr& series,
                                  const ::fwLog::Logger::sptr& logger = nullptr,
                                  ProgressCallback progress           = nullptr,
                                  CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~SpatialFiducials();

    /**
     * @brief Write Spatial Fiducials Module tags
     * @see PS 3.3 C.21.2
     */
    FWGDCMIO_API virtual void writeSpatialFiducialsModule();

    /**
     * @brief Write Common Instance Reference Module tags
     * @see PS 3.3 C.12.2
     */
    FWGDCMIO_API virtual void writeCommonInstanceReferenceModule();

    /**
     * @brief Write SOP Common Module tags
     * @see PS 3.3 C.12.1
     */
    FWGDCMIO_API virtual void writeSOPCommonModule();

protected:

    /**
     * @brief Write image landmarks into sequence
     * @throw(::fwGdcmIO::exception::Failed)
     */
    void writeLandmarks(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence);

    /**
     * @brief Write image distances into sequence
     */
    void writeDistances(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence);

    /**
     * @brief Add a frame number to the referenced image sequence
     * @param[in] frameNumber Frame Number
     * @param[in] referencedImageSequence Destination sequence
     * @throw(::fwGdcmIO::exception::Failed)
     */
    void addReferencedImage(int frameNumber,
                            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > referencedImageSequence);
};

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO

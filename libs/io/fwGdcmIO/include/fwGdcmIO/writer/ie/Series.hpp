/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwGdcmIO/writer/ie/InformationEntity.hpp"

#include <data/Series.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

/**
 * @brief Series Information Entity class
 */
class FWGDCMIO_CLASS_API Series : public ::fwGdcmIO::writer::ie::InformationEntity< data::Series >
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
    FWGDCMIO_API Series(const SPTR(::gdcm::Writer)& writer,
                        const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                        const data::Series::csptr& series,
                        const ::fwLog::Logger::sptr& logger = nullptr,
                        ProgressCallback progress           = nullptr,
                        CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~Series();

    /**
     * @brief Write General Series Module tags
     * @see PS 3.3 C.7.3.1
     */
    FWGDCMIO_API virtual void writeGeneralSeriesModule();

    /**
     * @brief Write Segmentation Series Module tags
     * @see PS 3.3 C.8.20.1
     */
    FWGDCMIO_API virtual void writeSegmentationSeriesModule();

    /**
     * @brief Write SR Document Series Module tags
     * @see PS 3.3 C.17.1
     */
    FWGDCMIO_API virtual void writeSRDocumentSeriesModule();

    /**
     * @brief Write Spatial Fiducials Series Module tags
     * @see PS 3.3 C.21.1
     */
    FWGDCMIO_API virtual void writeSpatialFiducialsSeriesModule();

};

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO

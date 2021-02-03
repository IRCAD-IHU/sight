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

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"
#include "fwGdcmIO/writer/iod/InformationObjectDefinition.hpp"

namespace fwGdcmIO
{
namespace writer
{
namespace iod
{

/**
 * @brief CTMRImageIOD class used to write CTImage and MRImage DICOM files
 */
class FWGDCMIO_CLASS_API CTMRImageIOD : public ::fwGdcmIO::writer::iod::InformationObjectDefinition
{

public:
    /**
     * @brief Constructor
     * @param[in] instance DICOM instance used to share information between modules
     * @param[in] destinationPath Destination path
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API CTMRImageIOD(const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                              const std::filesystem::path& destinationPath,
                              const ::fwLog::Logger::sptr& logger = nullptr,
                              ProgressCallback progress           = nullptr,
                              CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~CTMRImageIOD();

    /// Write DICOM file
    FWGDCMIO_API virtual void write(const data::Series::csptr& series) override;

};

} // namespace iod
} // namespace writer
} // namespace fwGdcmIO

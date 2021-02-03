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

#include <core/macros.hpp>

#include <gdcmDataSet.h>
#include <gdcmWriter.h>

#include <filesystem>

namespace fwGdcmIO
{
namespace helper
{

/**
 * @brief Helper used to write a DICOM file.
 */
class FWGDCMIO_CLASS_API FileWriter
{

public:
    /**
     * @brief Write a file from the dataset
     * @brief[in] filename Filename
     * @brief[in] writer GDCM writer
     */
    FWGDCMIO_API static void write(const std::filesystem::path& filename,
                                   const SPTR(::gdcm::Writer)& writer);
};

} // namespace helper
} // namespace fwGdcmIO

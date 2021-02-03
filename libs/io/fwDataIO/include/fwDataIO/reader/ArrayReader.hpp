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

#include "fwDataIO/config.hpp"
#include "fwDataIO/reader/GenericObjectReader.hpp"

#include <data/Array.hpp>
#include <data/location/SingleFile.hpp>

#include <filesystem>

namespace fwDataIO
{
namespace reader
{

/**
 * @brief   Array Reader. Read file format .raw
 *
 *
 * Ircad reader to read a data::Array on filesystem in a raw format.
 */
class FWDATAIO_CLASS_API ArrayReader :  public GenericObjectReader< data::Array >,
                                        public data::location::enableSingleFile< IObjectReader >
{

public:

    fwCoreClassMacro(ArrayReader, GenericObjectReader< data::Array>, ::fwDataIO::reader::factory::New<ArrayReader>)

    /// Constructor. Do nothing.
    FWDATAIO_API ArrayReader(::fwDataIO::reader::IObjectReader::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~ArrayReader();

    /// Read the file with zlib API.
    FWDATAIO_API virtual void read() override;

    /// Defines extension supported by this reader ".raw"
    FWDATAIO_API std::string extension() override;

};

} // namespace reader
} // namespace fwDataIO

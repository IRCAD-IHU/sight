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

#include "fwVtkIO/config.hpp"

#include <data/location/SingleFile.hpp>
#include <data/Mesh.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

#include <filesystem>

namespace fwJobs
{
class Observer;
class IJob;
}

namespace fwVtkIO
{

/**
 * @brief Reads a Mesh from .stl file using VTK library.
 *
 */

class StlMeshReader : public ::fwDataIO::reader::GenericObjectReader< data::Mesh >,
                      public data::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >
{

public:

    fwCoreClassMacro(StlMeshReader, ::fwDataIO::reader::GenericObjectReader< data::Mesh >,
                     ::fwDataIO::reader::factory::New< StlMeshReader >)
    fwCoreAllowSharedFromThis()

    /// @brief Constructor.
    FWVTKIO_API StlMeshReader(::fwDataIO::reader::IObjectReader::Key key);

    /// @brief Destructor.
    FWVTKIO_API ~StlMeshReader();

    /// @brief Reading operator.
    FWVTKIO_API void read() override;

    /// @return ".stl"
    FWVTKIO_API std::string extension() override;
    /// @return internal job
    FWVTKIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;

};

} // namespace fwVtkIO

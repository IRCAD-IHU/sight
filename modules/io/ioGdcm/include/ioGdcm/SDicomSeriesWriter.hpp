/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ioGdcm/config.hpp"

#include <fwIO/IWriter.hpp>

#include <filesystem>
#include <string>

namespace fwJobs
{
class IJob;
}

namespace sight::data
{
class DicomSeries;
}

namespace ioGdcm
{

/**
 * @brief DICOM Writer (DicomSeries)
 *
 * @section Signals Signals
 * - \b jobCreated( SPTR(::fwJobs::IJob) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="::ioGdcm::SDicomSeriesWriter" >
           <in key="data" uid="..." />
       </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b data [data::DicomSeries]: data to save in Dicom.
 */
class IOGDCM_CLASS_API SDicomSeriesWriter : public ::fwIO::IWriter
{

public:
    typedef core::com::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignal;

    fwCoreServiceMacro(SDicomSeriesWriter,  ::fwIO::IWriter)

    /**
     * @brief   constructor
     */
    IOGDCM_API SDicomSeriesWriter() noexcept;

    /**
     * @brief   destructor
     */
    IOGDCM_API virtual ~SDicomSeriesWriter() noexcept override;

    /**
     * @brief Propose select a directory where to save the DICOM files.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    IOGDCM_API virtual void configureWithIHM() override;

    /// Propose select a directory where to save the DICOM files.
    IOGDCM_API virtual void openLocationDialog() override;

protected:

    /// Does nothing
    IOGDCM_API virtual void starting() override;

    /// Does nothing
    IOGDCM_API virtual void stopping() override;

    /// Configuring method. This method is used to configure the service.
    IOGDCM_API virtual void configuring() override;

    /// Write the DicomSeries in DICOM format.
    IOGDCM_API void updating() override;

    /// Return path type managed by the service, here FOLDER
    IOGDCM_API ::fwIO::IOPathType getIOPathType() const override;

private:
    /// Save the selected Dicom series
    void saveDicomSeries( const std::filesystem::path folder,
                          const CSPTR(data::DicomSeries)& series ) const;

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;

    /// Cancel information for jobs
    bool m_cancelled;
};

} // namespace ioGdcm

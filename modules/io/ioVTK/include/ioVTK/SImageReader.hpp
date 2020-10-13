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

#include "ioVTK/config.hpp"  // Declaration of class and function export

#include <fwIO/IReader.hpp> // Definition of abstract reader class

#include <filesystem> // Used to save the file system path of loaded image

// Pre-definition of ::fwData::Image to avoid inclusion file
namespace fwData
{
class Image;
}

namespace fwJobs
{
class IJob;
}

namespace ioVTK
{

/**
 * @brief Service reading a VTK Image using the fwVtkIO lib.
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(::fwJobs::IJob)): emitted to display a progress bar while the image is loading (it should be
 * connected to a SJobBar).
 *
 * @section Slots Slots
 * - \b readFile(std::filesystem::path) : read the given file
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioVTK::SImageReader">
       <inout key="data" uid="..." />
       <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [::fwData::Image]: loaded image.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the image to load, if it is not defined, 'configureWithUI()' should be called to
 * define the path.
 */
class IOVTK_CLASS_API SImageReader : public ::fwIO::IReader
{

public:
    ~SImageReader() noexcept
    {
    }

    fwCoreServiceMacro(SImageReader,  ::fwIO::IReader)

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;

    /**
     * @brief Configure the image path with a dialogBox.
     *
     * This method is used to find the file path using a file selector.
     * @deprecated Will be removed in sight 22.0. Use configureWithUI() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use configureWithUI() instead.")]]
    IOVTK_API virtual void configureWithIHM() override;

    /**
     * @brief Configure the image path with a dialogBox.
     *
     * This method is used to find the file path using a file selector.
     */
    IOVTK_API virtual void configureWithUI() override;

    /**
     * @brief Constructor. Do nothing.
     */
    IOVTK_API SImageReader() noexcept;

    /**
     * @brief This method is used to load an vtk image using a file path.
     * @param[in] vtkFile file system path of vtk image
     * @param[out] image new empty image that will contain image loaded, if reading process is a success.
     * @return bool  \b true if the image loading is a success and \b false if it fails
     */
    IOVTK_API static bool loadImage( const std::filesystem::path& vtkFile,
                                     const SPTR(::fwData::Image)& image,
                                     const SPTR(JobCreatedSignalType)& sigJobCreated);

protected:

    IOVTK_API virtual ::fwIO::IOPathType getIOPathType() const override;

    /// Method called when the service is started, does nothing.
    IOVTK_API virtual void starting() override;

    /// Method called when the service is stopped, does nothing.
    IOVTK_API virtual void stopping() override;

    /// Method called when the service is stopped, does nothing.
    IOVTK_API virtual void configuring() override;

    /**
     * @brief Updating method execute the read process.
     *
     * This method is used to update the service.
     * The image is read.
     */
    IOVTK_API void updating() override;

    /// Info method gives some informations on service.
    IOVTK_API void info(std::ostream& _sstream ) override;

private:

    /// This value is \b true if the path image is known.
    bool m_bServiceIsConfigured;

    /// Image path, location of image on filesystem.
    std::filesystem::path m_fsImgPath;

    SPTR(JobCreatedSignalType) m_sigJobCreated;

};

} // namespace ioVTK

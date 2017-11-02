/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWVTKIO_BITMAPIMAGEREADER_HPP__
#define __FWVTKIO_BITMAPIMAGEREADER_HPP__

#include "fwVtkIO/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

#include <vector>

namespace fwJobs
{
class Observer;
class IJob;
}

namespace fwVtkIO
{

/**
 * @brief   Read a bitmap image.
 *
 * Read a Bitmap Image using the VTK library
 */
class BitmapImageReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >,
                          public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (BitmapImageReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >),
        (()),
        ::fwDataIO::reader::factory::New< BitmapImageReader >
        );

    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    FWVTKIO_API BitmapImageReader(::fwDataIO::reader::IObjectReader::Key key);

    //! @brief Destructor.
    FWVTKIO_API ~BitmapImageReader();

    //! @brief Reading operator.
    FWVTKIO_API void read() override;

    /// @return The available file extensions for loading bitmap images.
    FWVTKIO_API std::string extension() override;

    /// @return internal job
    FWVTKIO_API SPTR(::fwJobs::IJob) getJob() const override;

    /// @return A vector of the available bitmap extensions for the vtkImageReader2 class
    FWVTKIO_API static void getAvailableExtensions(std::vector<std::string>& ext);

private:

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;

    /// Available file extensions for bitmap files
    std::string m_availableExtensions;
};

} // namespace fwVtkIO

#endif // __FWVTKIO_BITMAPIMAGEREADER_HPP__

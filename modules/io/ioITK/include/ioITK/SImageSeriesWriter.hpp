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

#include "ioITK/config.hpp"

#include <fwIO/IWriter.hpp>

#include <filesystem>
#include <string>

namespace ioITK
{

/**
 * @brief Writer for .inr.gz image series
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioITK::SImageSeriesWriter">
       <in key="data" uid="..." />
       <file>...</file>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [::fwMedData::ImageSeries]: image series to save.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to save, if it not defined, 'configureWithUI()' should be called to define
 * the path.
 */
class IOITK_CLASS_API SImageSeriesWriter : public ::fwIO::IWriter
{

public:
    fwCoreServiceMacro(SImageSeriesWriter,  ::fwIO::IWriter)

    IOITK_API SImageSeriesWriter() noexcept;

    IOITK_API virtual ~SImageSeriesWriter() noexcept;

protected:

    /// Override
    IOITK_API virtual void starting() override;

    /// Override
    IOITK_API virtual void stopping() override;

    /// Override
    IOITK_API virtual void configuring() override;

    /// Override
    IOITK_API void updating() override;

    /// Override
    IOITK_API void info(std::ostream& _sstream ) override;

    /**
     * @brief Configure using GUI.
     * @deprecated Will be removed in sight 22.0. Use configureWithUI() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use configureWithUI() instead.")]]
    IOITK_API virtual void configureWithIHM() override;

    /// Configure using GUI.
    IOITK_API virtual void configureWithUI() override;

    /// Return managed file type, here FILE
    IOITK_API ::fwIO::IOPathType getIOPathType() const override;

};

} // namespace ioITK

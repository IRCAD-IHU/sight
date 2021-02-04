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

#include "ioITK/config.hpp"

#include <io/base/services/IWriter.hpp>

#include <filesystem>
#include <string>

namespace ioITK
{

/**
 * @brief Write an image to jpg format
 **/

/**
 * @brief Write an image series to jpg format
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioITK::SJpgImageSeriesWriter">
       <in key="data" uid="..." />
       <folder>...</folder>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [data::ImageSeries]: image series to save.
 * @subsection Configuration Configuration
 * - \b folder (optional): path of the folder, if it is not defined, 'openLocationDialog()' should be called to define
 * the path.
 */
class IOITK_CLASS_API SJpgImageSeriesWriter : public sight::io::base::services::IWriter
{

public:
    fwCoreServiceMacro(SJpgImageSeriesWriter, sight::io::base::services::IWriter)

    IOITK_API SJpgImageSeriesWriter() noexcept;

    IOITK_API virtual ~SJpgImageSeriesWriter() noexcept;

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
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    IOITK_API virtual void configureWithIHM() override;

    /// Configure using GUI.
    IOITK_API virtual void openLocationDialog() override;

    /// Return managed file type, here FOLDER
    IOITK_API io::base::services::IOPathType getIOPathType() const override;

};

} // namespace ioITK

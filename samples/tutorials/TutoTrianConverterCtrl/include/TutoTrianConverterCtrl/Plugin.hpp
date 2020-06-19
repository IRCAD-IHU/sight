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

#include "TutoTrianConverterCtrl/config.hpp"

#include <fwData/Mesh.hpp>

#include <fwRuntime/Plugin.hpp>

#include <fwServices/IService.hpp>

namespace TutoTrianConverterCtrl
{

/**
 * @brief   This class is started when the module is loaded.
 */
class TUTOTRIANCONVERTERCTRL_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTOTRIANCONVERTERCTRL_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTOTRIANCONVERTERCTRL_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTOTRIANCONVERTERCTRL_API void start();

    /// Overrides stop method. Do nothing
    TUTOTRIANCONVERTERCTRL_API void stop() noexcept;

    TUTOTRIANCONVERTERCTRL_API void initialize();

    TUTOTRIANCONVERTERCTRL_API void uninitialize() noexcept;

private:
    ::fwData::Mesh::sptr m_mesh;
    ::fwServices::IService::sptr m_writerSrv;
    ::fwServices::IService::sptr m_readerSrv;
};

} // namespace TutoTrianConverterCtrl

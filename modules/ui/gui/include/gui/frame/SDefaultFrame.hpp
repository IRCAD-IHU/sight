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

#include "gui/config.hpp"

#include <core/tools/Failed.hpp>

#include <fwGui/IFrameSrv.hpp>

namespace gui
{
namespace frame
{

/**
 * @brief Defines the default frame for standard application.
 */
class GUI_CLASS_API SDefaultFrame : public ::fwGui::IFrameSrv
{

public:

    fwCoreServiceMacro(SDefaultFrame, ::fwGui::IFrameSrv)

    /// Constructor. Do nothing.
    GUI_API SDefaultFrame() noexcept;

    /// Destructor. Do nothing.
    GUI_API virtual ~SDefaultFrame() noexcept;

protected:

    /** @name Service methods ( override from services::IService )
     * @{
     */

    /**
     * @brief This method is used to configure the class parameters.
     * @see ::fwGui::IFrameSrv::initialize()
     */
    GUI_API virtual void configuring() override;

    /**
     * @brief Register a view with defined id.
     * @see ::fwGui::IFrameSrv::create()
     */
    GUI_API virtual void starting() override;

    /**
     * @brief This method remove the view in the frame.
     * @see ::fwGui::IFrameSrv::destroy()
     */
    GUI_API virtual void stopping() override;

    /**
     * @brief This method is used to update services. Do nothing.
     */
    GUI_API virtual void updating() override;

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    GUI_API virtual void info(std::ostream& _sstream ) override;
    ///@}

};

} // namespace frame
} // namespace gui

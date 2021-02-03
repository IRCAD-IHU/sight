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

#include <fwGui/view/IView.hpp>

namespace gui
{
namespace view
{

/**
 * @brief Defines the generic layout for application.
 * @see ::fwGui::IGuiContainer for the service configuration
 */
class GUI_CLASS_API SDefaultView : public ::fwGui::view::IView
{
public:

    fwCoreServiceMacro(SDefaultView, ::fwGui::view::IView)

    /// Constructor. Do nothing.
    GUI_API SDefaultView() noexcept;

    /// Destructor. Do nothing.
    GUI_API virtual ~SDefaultView() noexcept;

protected:

    /**
     * @name Service methods ( override from ::fwServices::IService )
     * @{
     */

    /**
     * @brief This method is used to configure the class parameters.
     * @see ::fwGui::IGuiContainerSrv::initialize()
     */
    GUI_API virtual void configuring() override;

    /**
     * @brief Register a view with defined id.
     * @see ::fwGui::IGuiContainerSrv::create()
     */
    GUI_API virtual void starting() override;

    /**
     * @brief This method remove the view in the frame.
     * @see ::fwGui::IGuiContainerSrv::destroy()
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

} // namespace view
} // namespace gui

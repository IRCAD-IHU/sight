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

#include "modules/ui/base/config.hpp"

#include <ui/base/IMenuSrv.hpp>

namespace sight::modules::ui::base
{
namespace aspect
{

/**
 * @brief   Defines the default menu for standard application
 */
class MODULE_UI_BASE_CLASS_API SDefaultMenu : public ::sight::ui::base::IMenuSrv
{

public:

    fwCoreServiceMacro(SDefaultMenu, ::sight::ui::base::IMenuSrv)

    /// Constructor. Do nothing.
    MODULE_UI_BASE_API SDefaultMenu() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_BASE_API virtual ~SDefaultMenu() noexcept;

protected:

    /** @name Service methods ( override from services::IService )
     * @{
     */

    /**
     * @brief Configuring method allows to configure a menu with several actions.

     * Example of configuration
     * @code{.xml}
       <service uid="menuBar" type="::ui::base::IMenuBarSrv" impl="::modules::ui::base::aspect::SDefaultMenuBar"
     * autoConnect="no"
     *>
          <gui>
              <layout>
                  <menu name="My Menu"/>
                  <menu name="My Menu 2"/>
              </layout>
          </gui>
          <registry>
              <menu sid="myMenu" start="yes" />
              <menu sid="myMenu2" start="yes" />
          </registry>
       </service>
       @endcode
     *  - \<gui\> \</gui\> : (mandatory) describe the interface of the service.
     *    - <separator /> : allows to put a separator in the menu
     *  - \<registry\> \</registry\> : (mandatory) describe the service management.
     *
     * @warning
     * - The number of item in the gui section must be equal or greater than in the registry section.
     * - The order of the menu in each section (gui and registry) must be the same.\n
     *   For example: the menu named "My Menu" will be connected with the service which have the sid = "myMenu".
     * - A menu bar can't have the same service connected on two different menu.
     *
     *  @see sight::ui::base::IMenuBarSrv::initialize(),
     * sight::ui::base::layoutManager::IMenuLayoutManager::initialize()
     */
    MODULE_UI_BASE_API virtual void configuring() override;

    /**
     * @brief Create the menu items and start the managed services.
     * @see sight::ui::base::IMenuSrv::create()
     */
    MODULE_UI_BASE_API virtual void starting() override;

    /**
     * @brief Stop all actions and separator of this menu and remove menu to menuBar
     * @see sight::ui::base::IMenuSrv::destroy()
     */
    MODULE_UI_BASE_API virtual void stopping() override;

    /// Updating service, do nothing.
    MODULE_UI_BASE_API virtual void updating() override;
    ///@}

};

}

}

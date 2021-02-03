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

#include "fwGuiQt/config.hpp"
#include "fwGuiQt/container/QtContainer.hpp"

#include <core/base.hpp>

#include <fwGui/builder/IMenuBarBuilder.hpp>
#include <fwGui/container/fwContainer.hpp>
#include <fwGui/container/fwMenuBar.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
namespace fwGui
{
namespace builder
{

/**
 * @brief   Defines the generic layout manager for IHM.
 *
 */
class FWGUIQT_CLASS_API MenuBarBuilder : public ::fwGui::builder::IMenuBarBuilder
{
public:
    fwCoreClassMacro(MenuBarBuilder, ::fwGui::builder::IMenuBarBuilder, ::fwGui::factory::New< MenuBarBuilder >)

    FWGUIQT_API MenuBarBuilder(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~MenuBarBuilder();

    /**
     * @brief Instantiate layout with parent menuBar.
     * @pre LayoutManager must be initialized before.
     * @pre parent menuBar must be instanced.
     */
    FWGUIQT_API virtual void createMenuBar( ::fwGui::container::fwContainer::sptr parent ) override;

    /**
     * @brief Destroy local layout with sub containers.
     * @pre services using this sub containers must be stopped before.
     */
    FWGUIQT_API virtual void destroyMenuBar() override;

protected:
    ::fwGuiQt::container::QtContainer::sptr m_parent;
};

} // namespace builder
} // namespace fwGui

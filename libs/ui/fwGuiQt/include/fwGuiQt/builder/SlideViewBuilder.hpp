/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include <fwGui/builder/ISlideViewBuilder.hpp>
#include <fwGui/container/fwContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

namespace fwGui
{
namespace builder
{

/// This builder creates a slide widget. The slide widget can be displayed over all others widgets.
class FWGUIQT_CLASS_API SlideViewBuilder : public ::fwGui::builder::ISlideViewBuilder
{

public:

    fwCoreClassMacro(SlideViewBuilder, ::fwGui::builder::ISlideViewBuilder, ::fwGui::factory::New< SlideViewBuilder >)

    /// Creates the slide builder.
    FWGUIQT_API SlideViewBuilder(::fwGui::GuiBaseObject::Key key);

    /// Destroyes the slide builder.
    FWGUIQT_API virtual ~SlideViewBuilder();

    /// Create the Qt container @ref ::fwGuiQt::widget::SlideBar.
    FWGUIQT_API virtual void createContainer( ::fwGui::container::fwContainer::sptr parent ) override;

    /// Destroyes the container.
    FWGUIQT_API virtual void destroyContainer() override;

protected:

    /// Contains the generic parent container.
    ::fwGuiQt::container::QtContainer::sptr m_parent;

};

} // namespace builder.
} // namespace fwGui.

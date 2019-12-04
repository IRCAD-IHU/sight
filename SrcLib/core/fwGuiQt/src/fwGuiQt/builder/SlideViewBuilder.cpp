/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "fwGuiQt/builder/SlideViewBuilder.hpp"

#include "fwGuiQt/container/QtContainer.hpp"
#include <fwGuiQt/widget/SlideBar.hpp>

#include <fwGui/registry/macros.hpp>

#include <QWidget>

fwGuiRegisterMacro( ::fwGui::builder::SlideViewBuilder, ::fwGui::builder::ISlideViewBuilder::REGISTRY_KEY);

namespace fwGui
{
namespace builder
{

//-----------------------------------------------------------------------------

SlideViewBuilder::SlideViewBuilder(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

SlideViewBuilder::~SlideViewBuilder()
{
}

//-----------------------------------------------------------------------------

void SlideViewBuilder::createContainer( ::fwGui::container::fwContainer::sptr parent )
{
    m_parent = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("The parent container is not a QtContainer", m_parent);

    QWidget* qtParent = m_parent->getQtContainer();

    ::fwGuiQt::widget::SlideBar::Aligment align = ::fwGuiQt::widget::SlideBar::LEFT;
    switch (m_aligment)
    {
        case ISlideViewBuilder::LEFT:
            align = ::fwGuiQt::widget::SlideBar::LEFT;
            break;
        case ISlideViewBuilder::RIGHT:
            align = ::fwGuiQt::widget::SlideBar::RIGHT;
            break;
        case ISlideViewBuilder::TOP:
            align = ::fwGuiQt::widget::SlideBar::TOP;
            break;
        case ISlideViewBuilder::BOTTOM:
            align = ::fwGuiQt::widget::SlideBar::BOTTOM;
            break;
        default:
            SLM_ASSERT("Alignement not managed", false);
            break;
    }

    ::fwGuiQt::widget::SlideBar* slideBar = new ::fwGuiQt::widget::SlideBar(qtParent, align, m_size, m_opacity,
                                                                            m_animatable);
    if (!m_styleSheet.empty())
    {
        slideBar->setStyleSheet(QString::fromStdString(m_styleSheet));
    }

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::New();
    qtContainer->setQtContainer(slideBar);

    m_container = qtContainer;
}

//-----------------------------------------------------------------------------

void SlideViewBuilder::destroyContainer()
{
    SLM_ASSERT("The Container is not initialized", m_container);

    m_container->destroyContainer();
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwGui

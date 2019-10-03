/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGuiQt/layoutManager/ToolBarLayoutManager.hpp"

#include "fwGuiQt/ActionCallback.hpp"
#include "fwGuiQt/container/QtContainer.hpp"
#include "fwGuiQt/container/QtMenuContainer.hpp"
#include "fwGuiQt/container/QtMenuItemContainer.hpp"
#include "fwGuiQt/container/QtToolBarContainer.hpp"

#include <fwGui/registry/macros.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/lambda/lambda.hpp>

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>

#include <functional>

fwGuiRegisterMacro( ::fwGui::layoutManager::ToolBarLayoutManager,
                    ::fwGui::layoutManager::IToolBarLayoutManager::REGISTRY_KEY );

namespace fwGui
{
namespace layoutManager
{

//-----------------------------------------------------------------------------

ToolBarLayoutManager::ToolBarLayoutManager(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

ToolBarLayoutManager::~ToolBarLayoutManager()
{
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::createLayout( ::fwGui::container::fwToolBar::sptr parent)
{
    SLM_TRACE_FUNC();

    m_parent = ::fwGuiQt::container::QtToolBarContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwToolBar to QtToolBarContainer failed", m_parent);

    QToolBar* toolBar = m_parent->getQtToolBar();

    if(m_style == "ToolButtonTextOnly")
    {
        toolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    }
    else if(m_style == "ToolButtonTextBesideIcon")
    {
        toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    else if(m_style == "ToolButtonTextUnderIcon")
    {
        toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
    else if(m_style == "ToolButtonFollowStyle")
    {
        toolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    }
    else
    {
        toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }

    QActionGroup* actionGroup  = 0;
    unsigned int menuItemIndex = 0;
    for ( ::fwGui::layoutManager::IToolBarLayoutManager::ActionInfo actionInfo : m_actionInfo)
    {
        if (actionInfo.m_isSeparator)
        {
            if (actionInfo.m_size > 0)
            {
                QWidget* widget = new QWidget(toolBar);
                widget->setMinimumWidth(actionInfo.m_size);
                toolBar->addWidget(widget);
            }
            else
            {
                toolBar->addSeparator();
            }
            actionGroup = 0;
        }
        else if (actionInfo.m_isSpacer)
        {
            QWidget* spacer = new QWidget(toolBar);
            spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            toolBar->addWidget(spacer);
            actionGroup = 0;
        }
        else if (actionInfo.m_isMenu)
        {
            ::fwGuiQt::container::QtMenuContainer::sptr menu = ::fwGuiQt::container::QtMenuContainer::New();
            QMenu* qtMenu = new QMenu(toolBar);
            menu->setQtMenu(qtMenu);

            QToolButton* toolButton = new QToolButton(toolBar);
            if (toolBar->orientation() == Qt::Horizontal)
            {
                toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            }
            else
            {
                toolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            }
            toolButton->setMenu(qtMenu);
            toolButton->setPopupMode(QToolButton::InstantPopup);
            toolButton->setText(QString::fromStdString(actionInfo.m_name));
            if (!actionInfo.m_icon.empty())
            {
                QIcon icon(QString::fromStdString(actionInfo.m_icon.string()));
                toolButton->setIcon(icon);
                toolButton->setToolTip(QString::fromStdString(actionInfo.m_name));
                toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            }
            toolBar->addWidget(toolButton);
            m_menus.push_back(menu);
        }
        else if (actionInfo.m_isEditor)
        {
            ::fwGuiQt::container::QtContainer::sptr container = ::fwGuiQt::container::QtContainer::New();
            QWidget* widget = new QWidget(toolBar);
            container->setQtContainer(widget);

            if (toolBar->orientation() == Qt::Horizontal)
            {
                widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            }
            else
            {
                widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            }
            widget->adjustSize();
            toolBar->addWidget(widget);

            m_containers.push_back(container);
        }
        else
        {
            ::fwGuiQt::container::QtMenuItemContainer::sptr menuItem = ::fwGuiQt::container::QtMenuItemContainer::New();
            QAction* action;
            if (!actionInfo.m_icon.empty())
            {
                QIcon icon(QString::fromStdString(actionInfo.m_icon.string()));
                if (!actionInfo.m_icon2.empty())
                {
                    icon.addFile(QString::fromStdString(actionInfo.m_icon2.string()), QSize(), QIcon::Normal,
                                 QIcon::On);
                    icon.addFile(QString::fromStdString(actionInfo.m_icon2.string()), QSize(), QIcon::Active,
                                 QIcon::On);
                }
                action = toolBar->addAction( icon, QString::fromStdString(actionInfo.m_name) );
            }
            else
            {
                action = toolBar->addAction( QString::fromStdString(actionInfo.m_name) );
            }

            action->setCheckable(actionInfo.m_isCheckable || actionInfo.m_isRadio);

            if (actionInfo.m_isRadio)
            {
                if (!actionGroup)
                {
                    actionGroup = new QActionGroup(toolBar);
                }
                actionGroup->addAction(action);
            }

            // create shortcut
            if( !actionInfo.m_shortcut.empty() )
            {
                action->setShortcut(QKeySequence(QString::fromStdString(actionInfo.m_shortcut)));
            }

            menuItem->setQtMenuItem(action);

            m_menuItems.push_back(menuItem);
            OSLM_ASSERT("No callback found for menu" << actionInfo.m_name, menuItemIndex < m_callbacks.size());
            ::fwGui::IMenuItemCallback::sptr callback = m_callbacks.at(menuItemIndex);

            ::fwGuiQt::ActionCallback::sptr qtCallback = ::fwGuiQt::ActionCallback::dynamicCast(callback);
            SLM_ASSERT("dynamicCast IMenuItemCallback to ActionCallback failed", qtCallback);

            QObject::connect( action, SIGNAL(triggered(bool)), qtCallback.get(), SLOT(executeQt(bool)));
            QObject::connect( action, SIGNAL(toggled(bool)), qtCallback.get(), SLOT(checkQt(bool)));
            menuItemIndex++;
        }
    }
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::destroyLayout()
{
    this->destroyActions();
    m_parent->clean();
    m_menuItems.clear();
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetVisible(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isVisible)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer =
        ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetEnabled(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isEnabled)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer =
        ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

void ToolBarLayoutManager::menuItemSetChecked(::fwGui::container::fwMenuItem::sptr fwMenuItem, bool isChecked)
{
    ::fwGuiQt::container::QtMenuItemContainer::sptr menuItemContainer =
        ::fwGuiQt::container::QtMenuItemContainer::dynamicCast(fwMenuItem);
    QAction* action = menuItemContainer->getQtMenuItem();
    action->setChecked(isChecked);
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui

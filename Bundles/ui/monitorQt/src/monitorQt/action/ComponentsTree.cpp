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

#include "monitorQt/action/ComponentsTree.hpp"

#include <fwCore/base.hpp>

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Extension.hpp>
#include <fwRuntime/Runtime.hpp>

#include <fwServices/macros.hpp>

#include <QApplication>
#include <QVBoxLayout>

namespace monitorQt
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::monitorQt::action::ComponentsTree, ::fwData::Object )

//------------------------------------------------------------------------------

ComponentsTree::ComponentsTree( ) noexcept
{
}

//------------------------------------------------------------------------------

ComponentsTree::~ComponentsTree() noexcept
{
}

//------------------------------------------------------------------------------

void ComponentsTree::updating( )
{
    m_treeContainer->clearSelection();
    m_treeContainer->clear();

    ::fwRuntime::Runtime* defaultRuntime = ::fwRuntime::Runtime::getDefault();

    for (const auto& bundle : defaultRuntime->getBundles())
    {
        const std::string bundleName      = bundle->getIdentifier();
        const bool isBundleEnabled        = bundle->isEnable();
        QTreeWidgetItem* const moduleItem = new QTreeWidgetItem();
        if(!isBundleEnabled)
        {
            moduleItem->setBackground(0, QBrush(QColor(155, 155, 155)));
        }
        moduleItem->setText(0, QString::fromStdString(bundleName));
        m_treeContainer->addTopLevelItem( moduleItem );

        //Extensions
        QTreeWidgetItem* extensionsItem = new QTreeWidgetItem();
        extensionsItem->setText(0, QObject::tr("Extensions"));
        moduleItem->addChild( extensionsItem );

        for (const auto& extension : bundle->getExtensions())
        {
            const std::string point       = extension->getPoint();
            const bool isExtensionEnabled = extension->isEnable();
            QTreeWidgetItem* const item   = new QTreeWidgetItem();
            if(!isExtensionEnabled)
            {
                item->setBackground(0, QBrush(QColor(155, 155, 155)));
            }
            item->setText(0, QString::fromStdString(point));
            extensionsItem->addChild( item );
        }
    }

    m_dialog->show();
}

//------------------------------------------------------------------------------

void ComponentsTree::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void ComponentsTree::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();

    QWidget* parent = qApp->activeWindow();
    m_dialog = new QDialog(parent);
    m_dialog->setWindowTitle("Components Tree");
    m_treeContainer = new QTreeWidget(m_dialog);
    m_treeContainer->setHeaderHidden(true);

    QVBoxLayout* topsizer = new QVBoxLayout();
    topsizer->addWidget(m_treeContainer);
    topsizer->setContentsMargins(0, 0, 0, 0);
    m_dialog->setLayout(topsizer);
}

//------------------------------------------------------------------------------
void ComponentsTree::stopping()
{
    m_dialog->hide();
    delete m_treeContainer;
    delete m_dialog;

    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action

} // namespace monitorQt

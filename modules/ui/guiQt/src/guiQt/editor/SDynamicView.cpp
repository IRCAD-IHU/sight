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

#include "guiQt/editor/SDynamicView.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/UUID.hpp>

#include <fwActivities/IActivityValidator.hpp>
#include <fwActivities/IValidator.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Composite.hpp>
#include <fwData/reflection/getObject.hpp>
#include <fwData/String.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/GuiRegistry.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <boost/foreach.hpp>

#include <QBoxLayout>
#include <QTabWidget>
#include <QtGui>

#include <regex>

namespace guiQt
{
namespace editor
{

static const core::com::Slots::SlotKeyType s_CREATE_TAB_SLOT = "createTab";

static const core::com::Signals::SignalKeyType s_ACTIVITY_SELECTED_SLOT = "activitySelected";
static const core::com::Signals::SignalKeyType s_NOTHING_SELECTED_SLOT  = "nothingSelected";

fwServicesRegisterMacro( ::fwGui::view::IActivityView, ::guiQt::editor::SDynamicView )

//------------------------------------------------------------------------------

SDynamicView::SDynamicView() noexcept :
    m_mainActivityClosable(true)
{
    m_dynamicConfigStartStop = false;

    newSlot(s_CREATE_TAB_SLOT, &SDynamicView::createTab, this);

    m_sigActivitySelected = newSignal< ActivitySelectedSignalType >(s_ACTIVITY_SELECTED_SLOT);
    m_sigNothingSelected  = newSignal< NothingSelectedSignalType >(s_NOTHING_SELECTED_SLOT);
}

//------------------------------------------------------------------------------

SDynamicView::~SDynamicView() noexcept
{
}

//------------------------------------------------------------------------------

void SDynamicView::configuring()
{
    this->::fwGui::view::IActivityView::configuring();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigType;

    ConfigType activityConfig = m_configuration->findConfigurationElement("mainActivity");
    if (activityConfig)
    {
        const std::string closableStr = activityConfig->getAttributeValue("closable");
        SLM_ASSERT("main activity 'closable' attribute value must be 'yes', 'true', 'no' or 'false'",
                   closableStr == "yes" || closableStr == "true" ||
                   closableStr == "no" || closableStr == "false");
        const bool closable = (closableStr == "yes" || closableStr == "true");
        m_mainActivityClosable = closable;
    }
    ConfigType config = m_configuration->findConfigurationElement("config");
    if(config)
    {
        const std::string documentStr = config->getAttributeValue("document");
        if(!documentStr.empty())
        {
            SLM_ASSERT("'document' attribute value must be 'true' or 'false'",
                       documentStr == "true" || documentStr == "false");
            const bool document = documentStr == "true";
            m_documentMode = document;
        }
    }
}

//------------------------------------------------------------------------------

void SDynamicView::starting()
{
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr parentContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    m_tabWidget = new QTabWidget();
    m_tabWidget->setTabsClosable( true );
    m_tabWidget->setDocumentMode( m_documentMode );
    m_tabWidget->setMovable( true );

    QObject::connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTabSignal(int)));
    QObject::connect(m_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(changedTab(int)));

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->addWidget( m_tabWidget );

    parentContainer->setLayout(layout);

    m_currentWidget = 0;

    if (!m_mainActivityId.empty())
    {
        this->buildMainActivity();
    }
}

//------------------------------------------------------------------------------

void SDynamicView::stopping()
{
    while(m_tabWidget->count())
    {
        this->closeTab(0, true);
    }
    m_tabWidget->clear();

    this->destroy();
    m_tabWidget = 0;
}

//------------------------------------------------------------------------------

void SDynamicView::updating()
{
}

//------------------------------------------------------------------------------

void SDynamicView::swapping()
{
}

//------------------------------------------------------------------------------

void SDynamicView::launchActivity(::fwMedData::ActivitySeries::sptr activitySeries)
{
    if (this->validateActivity(activitySeries))
    {
        SDynamicViewInfo viewInfo = this->createViewInfo(activitySeries);
        viewInfo.closable = true;

        this->launchTab(viewInfo);
    }
}

//------------------------------------------------------------------------------

void SDynamicView::createTab( ::fwActivities::registry::ActivityMsg info )
{
    SDynamicViewInfo viewInfo;
    viewInfo.title          = info.getTitle();
    viewInfo.tabID          = info.getTabID();
    viewInfo.closable       = info.isClosable();
    viewInfo.icon           = info.getIconPath();
    viewInfo.tooltip        = info.getToolTip();
    viewInfo.viewConfigID   = info.getAppConfigID();
    viewInfo.replaceMap     = info.getReplaceMap();
    viewInfo.activitySeries = info.getActivitySeries();

    this->launchTab(viewInfo);
}

//------------------------------------------------------------------------------

void SDynamicView::launchTab(SDynamicViewInfo& info)
{
    static int count              = 0;
    ActivityIdType::iterator iter = std::find(m_activityIds.begin(), m_activityIds.end(), info.activitySeries->getID());

    if (iter != m_activityIds.end())
    {
        ::fwGui::dialog::MessageDialog::show("Launch Activity",
                                             "The current activity is already launched. \n"
                                             "It cannot be launched twice.",
                                             ::fwGui::dialog::IMessageDialog::WARNING);
        return;
    }

    if ( m_titleToCount.find( info.title ) != m_titleToCount.end() )
    {
        m_titleToCount[ info.title ]++;
    }
    else
    {
        m_titleToCount[ info.title ] = 1;
    }

    QString finalTitle = QString("%1 %2").arg( info.title.c_str(), "(%1)" ).arg( m_titleToCount[ info.title ] );
    info.wid = QString("SDynamicView-%1").arg(count++).toStdString();

    ::fwGuiQt::container::QtContainer::sptr subContainer = ::fwGuiQt::container::QtContainer::New();
    QWidget* widget = new QWidget(m_tabWidget);
    subContainer->setQtContainer(widget);
    ::fwGui::GuiRegistry::registerWIDContainer(info.wid, subContainer);

    info.replaceMap[ "WID_PARENT" ] = info.wid;
    std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier(info.viewConfigID);
    info.replaceMap["GENERIC_UID"] = genericUidAdaptor;

    ::fwServices::IAppConfigManager::sptr helper = ::fwServices::IAppConfigManager::New();

    try
    {
        helper->setConfig( info.viewConfigID, info.replaceMap );
        if (!m_dynamicConfigStartStop)
        {
            helper->launch();
        }
        else
        {
            helper->create();
        }
    }
    catch( std::exception& e )
    {
        ::fwGui::dialog::MessageDialog::show("Activity launch failed",
                                             e.what(),
                                             ::fwGui::dialog::IMessageDialog::CRITICAL);
        SLM_ERROR(e.what());
        return;
    }

    info.container = subContainer;
    info.helper    = helper;
    m_activityIds.insert(info.activitySeries->getID());

    m_dynamicInfoMap[widget] = info;
    m_tabIDList.insert(info.tabID);

    int index = m_tabWidget->addTab(widget, finalTitle );
    if(!info.tooltip.empty())
    {
        m_tabWidget->setTabToolTip(index, QString::fromStdString(info.tooltip));
    }
    if(!info.icon.empty())
    {
        m_tabWidget->setTabIcon(index, QIcon(QString::fromStdString(info.icon)) );
    }
    m_tabWidget->setCurrentWidget(widget);
}

//------------------------------------------------------------------------------

void SDynamicView::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

void SDynamicView::closeTabSignal( int index )
{
    closeTab( index, false );
}

//------------------------------------------------------------------------------

void SDynamicView::closeTab( int index, bool forceClose )
{
    QWidget* widget = m_tabWidget->widget(index);

    SLM_ASSERT("Widget is not in dynamicInfoMap", m_dynamicInfoMap.find(widget) != m_dynamicInfoMap.end());
    SDynamicViewInfo info = m_dynamicInfoMap[widget];
    if ( info.closable || forceClose )
    {
        m_tabIDList.erase(info.tabID);
        if (!m_dynamicConfigStartStop)
        {
            info.helper->stopAndDestroy();
        }
        else
        {
            if (info.helper->isStarted())
            {
                info.helper->stop();
            }
            info.helper->destroy();
        }
        info.helper.reset();

        //Remove tab first, to avoid tab beeing removed by container->destroy
        m_currentWidget = 0;
        m_tabWidget->removeTab(index);

        ::fwGui::GuiRegistry::unregisterWIDContainer(info.wid);

        info.container->destroyContainer();
        info.container.reset();
        m_dynamicInfoMap.erase(widget);
        m_activityIds.erase(info.activitySeries->getID());
    }
    else
    {
        ::fwGui::dialog::MessageDialog::show("Close tab",
                                             "The tab " + info.title + " can not be closed.",
                                             ::fwGui::dialog::IMessageDialog::INFO);
    }
}

//------------------------------------------------------------------------------

void SDynamicView::changedTab( int index )
{
    QWidget* widget = m_tabWidget->widget(index);

    if (m_dynamicConfigStartStop && widget != m_currentWidget)
    {
        if (m_currentWidget)
        {
            SDynamicViewInfo oldinfo = m_dynamicInfoMap[m_currentWidget];
            oldinfo.helper->stop();
        }

        if (widget)
        {
            SDynamicViewInfo newinfo = m_dynamicInfoMap[widget];
            if (!newinfo.helper->isStarted())
            {
                newinfo.helper->start();
                newinfo.helper->update();
            }
        }
    }
    m_currentWidget = widget;

    if (index >= 0)
    {
        SDynamicViewInfo info = m_dynamicInfoMap[widget];
        m_sigActivitySelected->asyncEmit(info.activitySeries);
    }
    else
    {
        m_sigNothingSelected->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SDynamicView::buildMainActivity()
{
    ::fwMedData::ActivitySeries::sptr actSeries = this->createMainActivity();

    if (actSeries)
    {
        SDynamicViewInfo viewInfo;
        viewInfo          = this->createViewInfo(actSeries);
        viewInfo.closable = m_mainActivityClosable;

        this->launchTab(viewInfo);
    }
}

//------------------------------------------------------------------------------

SDynamicView::SDynamicViewInfo SDynamicView::createViewInfo(::fwMedData::ActivitySeries::sptr activitySeries)
{
    ReplaceMapType replaceMap;
    this->translateParameters(m_parameters, replaceMap);

    ::fwActivities::registry::ActivityInfo info;
    info = ::fwActivities::registry::Activities::getDefault()->getInfo(activitySeries->getActivityConfigId());

    std::string tabInfo;
    if(info.tabInfo.empty())
    {
        tabInfo = info.title;
    }
    else
    {
        std::string newTabInfo = info.tabInfo;
        std::regex e("(!(([\\w]+\\.?)+[\\w]))");
        std::smatch what;
        if(std::regex_search(newTabInfo, what, e))
        {
            std::string submatch(what[1].first, what[1].second);

            submatch.replace(0, 1, "@");

            ::fwData::Object::sptr obj = ::fwData::reflection::getObject(activitySeries->getData(), submatch);
            SLM_ASSERT("Invalid seshat path : '" << submatch <<"'", obj);

            ::fwData::String::sptr stringParameter = ::fwData::String::dynamicCast(obj);

            std::string tabInfoSeshat;

            if(stringParameter)
            {
                tabInfoSeshat = stringParameter->getValue();
            }
            else
            {
                SLM_WARN("Seshat path '" << submatch << "' doesn't reference an fwData::String");
            }

            submatch.replace(0, 1, "!");
            ::boost::algorithm::replace_all(newTabInfo, submatch, tabInfoSeshat);

        }
        tabInfo = newTabInfo;
    }

    this->translateParameters(activitySeries->getData(), info.appConfig.parameters, replaceMap);
    replaceMap["AS_UID"] = activitySeries->getID();

    SDynamicViewInfo viewInfo;
    viewInfo.title          = info.title;
    viewInfo.icon           = info.icon;
    viewInfo.tooltip        = tabInfo;
    viewInfo.viewConfigID   = info.appConfig.id;
    viewInfo.activitySeries = activitySeries;
    viewInfo.replaceMap     = replaceMap;

    return viewInfo;
}

//------------------------------------------------------------------------------

}// namespace editor
}// namespace guiQt

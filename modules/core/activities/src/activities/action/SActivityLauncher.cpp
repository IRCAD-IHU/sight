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

#include "activities/action/SActivityLauncher.hpp"

#include <fwActivities/IActivityValidator.hpp>
#include <fwActivities/IBuilder.hpp>
#include <fwActivities/IValidator.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/IAppConfigManager.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwTools/UUID.hpp>

#include <boost/foreach.hpp>

#ifdef KEEP_OLD_SERVICE

#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStandardItemModel>
#include <QVBoxLayout>

Q_DECLARE_METATYPE(::fwActivities::registry::ActivityInfo)

#endif

namespace activities
{
namespace action
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::activities::action::SActivityLauncher, ::fwData::Vector )

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SActivityLauncher::s_LAUNCH_SERIES_SLOT = "launchSeries";
const ::fwCom::Slots::SlotKeyType SActivityLauncher::s_LAUNCH_ACTIVITY_SERIES_SLOT = "launchActivitySeries";
const ::fwCom::Slots::SlotKeyType SActivityLauncher::s_UPDATE_STATE_SLOT           = "updateState";
const ::fwCom::Signals::SignalKeyType SActivityLauncher::s_ACTIVITY_LAUNCHED_SIG   = "activityLaunched";

static const ::fwServices::IService::KeyType s_SERIES_INPUT = "series";

//------------------------------------------------------------------------------

SActivityLauncher::SActivityLauncher() noexcept :
    m_mode("message")
{
#ifndef KEEP_OLD_SERVICE
    SLM_FATAL("Use '::uiActivitiesQt::action::SActivityLauncher' instead of '::activities::action::SActivityLauncher'");
#else
    FW_DEPRECATED("::activities::action::SActivityLauncher", "::uiActivitiesQt::action::SActivityLauncher", "21.0");
    m_sigActivityLaunched = newSignal< ActivityLaunchedSignalType >(s_ACTIVITY_LAUNCHED_SIG);

    newSlot(s_LAUNCH_SERIES_SLOT, &SActivityLauncher::launchSeries, this);
    newSlot(s_LAUNCH_ACTIVITY_SERIES_SLOT, &SActivityLauncher::launchActivitySeries, this);
    newSlot(s_UPDATE_STATE_SLOT, &SActivityLauncher::updateState, this);
#endif
}

//------------------------------------------------------------------------------

SActivityLauncher::~SActivityLauncher() noexcept
{
}

//------------------------------------------------------------------------------

void SActivityLauncher::starting()
{
    this->actionServiceStarting();
    this->updateState();
}

//------------------------------------------------------------------------------

void SActivityLauncher::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SActivityLauncher::configuring()
{
#ifdef KEEP_OLD_SERVICE
    this->::fwGui::IActionSrv::initialize();
    typedef ::fwServices::IService::ConfigType ConfigType;

    m_parameters.clear();
    if(this->getConfigTree().count("config") > 0)
    {
        SLM_ASSERT("There must be one (and only one) <config/> element.",
                   this->getConfigTree().count("config") == 1 );

        const ::fwServices::IService::ConfigType srvconfig = this->getConfigTree();
        const ::fwServices::IService::ConfigType& config   = srvconfig.get_child("config");

        m_mode = config.get_optional<std::string>("mode").get_value_or("message");
        SLM_ASSERT("SActivityLauncher mode must be either 'immediate' or 'message'",
                   "message" == m_mode || "immediate" == m_mode);

        if(config.count("parameters") == 1 )
        {
            const ::fwServices::IService::ConfigType& configParameters = config.get_child("parameters");
            BOOST_FOREACH( const ConfigType::value_type& v,  configParameters.equal_range("parameter") )
            {
                ParametersType::value_type parameter( v.second );
                m_parameters.push_back( parameter );
            }
        }
        SLM_ASSERT("A maximum of 1 <parameters> tag is allowed", config.count("parameters") < 2);

        if(config.count("filter") == 1 )
        {
            const ::fwServices::IService::ConfigType& configFilter = config.get_child("filter");
            SLM_ASSERT("A maximum of 1 <mode> tag is allowed", configFilter.count("mode") < 2);

            const std::string mode = configFilter.get< std::string >("mode");
            SLM_ASSERT("'" << mode << "' value for <mode> tag isn't valid. Allowed values are : 'include', 'exclude'.",
                       mode == "include" || mode == "exclude");
            m_filterMode = mode;

            BOOST_FOREACH( const ConfigType::value_type& v,  configFilter.equal_range("id") )
            {
                m_keys.push_back(v.second.get<std::string>(""));
            }
        }
        SLM_ASSERT("A maximum of 1 <filter> tag is allowed", config.count("filter") < 2);

        if(config.count("quickLaunch") == 1 )
        {
            m_quickLaunch.clear();
            const ::fwServices::IService::ConfigType& configQuickLaunch = config.get_child("quickLaunch");
            BOOST_FOREACH( const ConfigType::value_type& v,  configQuickLaunch.equal_range("association") )
            {
                const ::fwServices::IService::ConfigType& association = v.second;
                const ::fwServices::IService::ConfigType xmlattr      = association.get_child("<xmlattr>");

                SLM_FATAL_IF( "The attribute \"type\" is missing", xmlattr.count("type") != 1 );
                SLM_FATAL_IF( "The attribute \"id\" is missing", xmlattr.count("id") != 1 );

                std::string type = xmlattr.get<std::string>("type");
                std::string id   = xmlattr.get<std::string>("id");

                m_quickLaunch[type] = id;
            }
        }
        SLM_ASSERT("A maximum of 1 <quickLaunch> tag is allowed", config.count("quickLaunch") < 2);
    }
#endif
}

//------------------------------------------------------------------------------

::fwActivities::registry::ActivityInfo SActivityLauncher::show( const ActivityInfoContainer& infos )
{
#ifdef KEEP_OLD_SERVICE
    QWidget* parent = qApp->activeWindow();

    QDialog* dialog = new QDialog(parent);
    dialog->setWindowTitle(QString::fromStdString("Choose an activity"));

    QStandardItemModel* model = new QStandardItemModel(dialog);
    for( ::fwActivities::registry::ActivityInfo info :  infos)
    {
        std::string text;
        if(info.title.empty())
        {
            text = info.id;
        }
        else
        {
            text = info.title + (info.description.empty() ? "" : "\n" + info.description);
        }

        QStandardItem* item = new QStandardItem(QIcon(info.icon.c_str()), QString::fromStdString(text));
        item->setData(QVariant::fromValue(info));
        item->setEditable(false);
        model->appendRow(item);
    }

    QListView* selectionList = new QListView();
    selectionList->setIconSize(QSize(100, 100));
    selectionList->setUniformItemSizes(true);
    selectionList->setModel(model);

    QModelIndex index = model->index( 0, 0 );
    if ( index.isValid() )
    {
        selectionList->selectionModel()->select( index, QItemSelectionModel::Select );
    }

    QPushButton* okButton     = new QPushButton("Ok");
    QPushButton* cancelButton = new QPushButton("Cancel");

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(okButton);
    hLayout->addWidget(cancelButton);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(selectionList);
    vLayout->addLayout(hLayout);

    dialog->setLayout(vLayout);
    QObject::connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    QObject::connect(selectionList, SIGNAL(doubleClicked(const QModelIndex&)), dialog, SLOT(accept()));

    ::fwActivities::registry::ActivityInfo info;
    if(dialog->exec())
    {
        QModelIndex currentIndex = selectionList->selectionModel()->currentIndex();
        QStandardItem* item      = model->itemFromIndex( currentIndex );
        QVariant var             = item->data();
        info = var.value< ::fwActivities::registry::ActivityInfo >();
    }

    return info;
#endif
    return ::fwActivities::registry::ActivityInfo();
}

//------------------------------------------------------------------------------

SActivityLauncher::ActivityInfoContainer SActivityLauncher::getEnabledActivities(const ActivityInfoContainer& infos)
{
    ActivityInfoContainer configs;

    if(m_filterMode == "include" || m_filterMode == "exclude")
    {
        const bool isIncludeMode = m_filterMode == "include";

        for(ActivityInfoContainer::const_iterator iter = infos.begin(); iter != infos.end(); ++iter)
        {
            KeysType::iterator keyIt = std::find(m_keys.begin(), m_keys.end(), iter->id);

            if(keyIt != m_keys.end() && isIncludeMode)
            {
                configs.push_back(*iter);
            }
            else if(keyIt == m_keys.end() && !isIncludeMode)
            {
                configs.push_back(*iter);
            }
        }
    }
    else
    {
        configs = infos;
    }

    return configs;
}

//------------------------------------------------------------------------------

void SActivityLauncher::updating()
{
    ::fwData::Vector::csptr selection = this->getInput< ::fwData::Vector >(s_SERIES_INPUT);
    SLM_ASSERT("The input key '" + s_SERIES_INPUT + "' is not correctly set.", selection);

    bool launchAS = this->launchAS(selection);
    if (!launchAS)
    {
        ActivityInfoContainer infos = ::fwActivities::registry::Activities::getDefault()->getInfos(selection);
        infos = this->getEnabledActivities(infos);

        if ( !infos.empty())
        {
            ::fwActivities::registry::ActivityInfo info;
            if((m_keys.size() == 1 && m_filterMode == "include") || (infos.size() == 1))
            {
                info = infos[0];
            }
            else
            {
                info = this->show( infos );
            }

            if( !info.id.empty() )
            {
                this->sendConfig( info );
            }
        }
        else
        {
            ::fwGui::dialog::MessageDialog::show("Activity launcher",
                                                 "Not available activity for the current selection.",
                                                 ::fwGui::dialog::MessageDialog::WARNING);
        }
    }
}

//------------------------------------------------------------------------------

void SActivityLauncher::updateState()
{
    ::fwData::Vector::csptr selection = this->getInput< ::fwData::Vector >(s_SERIES_INPUT);
    SLM_ASSERT("The input key '" + s_SERIES_INPUT + "' is not correctly set.", selection);

    bool isExecutable = false;

    if(selection->size() == 1 && ::fwMedData::ActivitySeries::dynamicCast((*selection)[0]))
    {
        ::fwMedData::ActivitySeries::sptr as = ::fwMedData::ActivitySeries::dynamicCast((*selection)[0]);

        if(m_filterMode == "include" || m_filterMode == "exclude")
        {
            const bool isIncludeMode = m_filterMode == "include";

            KeysType::iterator keyIt = std::find(m_keys.begin(), m_keys.end(), as->getActivityConfigId());

            if(keyIt != m_keys.end() && isIncludeMode)
            {
                isExecutable = true;
            }
            else if(keyIt == m_keys.end() && !isIncludeMode)
            {
                isExecutable = true;
            }
            isExecutable &= ::fwActivities::registry::Activities::getDefault()->hasInfo(
                as->getActivityConfigId());
        }
        else
        {
            isExecutable = ::fwActivities::registry::Activities::getDefault()->hasInfo(
                as->getActivityConfigId());
        }
    }
    else
    {
        ::fwActivities::registry::ActivityInfo::DataCountType dataCount;
        dataCount = ::fwActivities::registry::Activities::getDefault()->getDataCount(selection);
        if(m_filterMode.empty() && dataCount.size() == 1)
        {
            ::fwData::Object::sptr obj = selection->front();
            if (::fwMedData::ActivitySeries::dynamicCast(obj))
            {
                isExecutable = true;
            }
        }

        ActivityInfoContainer infos = ::fwActivities::registry::Activities::getDefault()->getInfos(selection);
        infos = this->getEnabledActivities(infos);

        isExecutable |= !infos.empty();
    }

    this->setIsExecutable(isExecutable);
}

//------------------------------------------------------------------------------

void SActivityLauncher::buildActivity(const ::fwActivities::registry::ActivityInfo& info,
                                      const ::fwData::Vector::csptr& selection)
{
    ::fwData::Composite::sptr replaceMap = ::fwData::Composite::New();
    ::fwActivities::IBuilder::sptr builder;
    builder = ::fwActivities::builder::factory::New(info.builderImpl);
    SLM_ASSERT(info.builderImpl << " instantiation failed", builder);

    ::fwMedData::ActivitySeries::sptr actSeries;
    actSeries = builder->buildData(info, selection);

    if( !actSeries )
    {
        std::string msg = "The activity <" + info.title + "> can't be launched. Builder <" + info.builderImpl +
                          "> failed.";
        ::fwGui::dialog::MessageDialog::show( "Activity can not be launched", msg,
                                              ::fwGui::dialog::IMessageDialog::WARNING);
        SLM_ERROR(msg);
        return;
    }

    // Applies activity validator on activity series to check the data
    if (!info.validatorsImpl.empty())
    {
        for (std::string validatorImpl : info.validatorsImpl)
        {
            /// Process activity validator
            ::fwActivities::IValidator::sptr validator = ::fwActivities::validator::factory::New(validatorImpl);

            ::fwActivities::IActivityValidator::sptr activityValidator =
                ::fwActivities::IActivityValidator::dynamicCast(validator);

            if (activityValidator)
            {
                ::fwActivities::IValidator::ValidationType validation = activityValidator->validate(actSeries);
                if(!validation.first)
                {
                    std::string message = "The activity '" + info.title + "' can not be launched:\n" +
                                          validation.second;
                    ::fwGui::dialog::MessageDialog::show("Activity launch",
                                                         message,
                                                         ::fwGui::dialog::IMessageDialog::CRITICAL);
                    return;
                }
            }
        }
    }

    ParametersType parameters = this->translateParameters(m_parameters);
    ::fwActivities::registry::ActivityMsg msg = ::fwActivities::registry::ActivityMsg(actSeries, info, parameters);

    if( m_mode == "message" )
    {
        m_sigActivityLaunched->asyncEmit(msg);
    }
    else
    {
        ::fwGui::LockAction lock(this->getSptr());

        std::string viewConfigID = msg.getAppConfigID();
        ::fwActivities::registry::ActivityMsg::ReplaceMapType replaceMap = msg.getReplaceMap();
        replaceMap["GENERIC_UID"]                                        =
            ::fwServices::registry::AppConfig::getUniqueIdentifier();

        ::fwServices::IAppConfigManager::sptr helper = ::fwServices::IAppConfigManager::New();
        helper->setConfig( viewConfigID, replaceMap );
        helper->launch();
        helper->stopAndDestroy();
    }
}

//------------------------------------------------------------------------------

void SActivityLauncher::sendConfig( const ::fwActivities::registry::ActivityInfo& info )
{
    // Start module containing the activity if it is not started
    std::shared_ptr< ::fwRuntime::Module > module = ::fwRuntime::findModule(info.bundleId, info.bundleVersion);
    SLM_WARN_IF("Module '" + info.bundleId + "' used by activity '" + info.id + "' is already started.",
                module->isStarted());
    if (!module->isStarted())
    {
        SLM_DEBUG("Start module '" + info.bundleId + "' used by activity '" + info.id + "'");
        module->start();
    }

    ::fwData::Vector::csptr selection = this->getInput< ::fwData::Vector >(s_SERIES_INPUT);
    SLM_ASSERT("The input key '" + s_SERIES_INPUT + "' is not correctly set.", selection);

    ::fwActivities::IValidator::ValidationType validation;
    validation.first = true;

    for(auto const& validatorImpl :  info.validatorsImpl)
    {
        ::fwActivities::IValidator::sptr validator = ::fwActivities::validator::factory::New(validatorImpl);
        SLM_ASSERT(validatorImpl << " instantiation failed", validator);

        ::fwActivities::IValidator::ValidationType valid = validator->validate(info, selection);
        validation.first                                &= valid.first;
        if(!valid.first)
        {
            validation.second += "\n" + valid.second;
        }
    }

    if(!validation.first)
    {
        ::fwGui::dialog::MessageDialog::show(
            "Activity can not be launched",
            "The activity " + info.title + " can't be launched. Reason : " + validation.second,
            ::fwGui::dialog::IMessageDialog::WARNING
            );
    }
    else
    {
        this->buildActivity(info, selection);
    }
}

//------------------------------------------------------------------------------

bool SActivityLauncher::launchAS(const ::fwData::Vector::csptr& selection)
{
    bool launchAS = false;
    ::fwActivities::registry::ActivityInfo::DataCountType dataCount;
    dataCount = ::fwActivities::registry::Activities::getDefault()->getDataCount(selection);
    if(dataCount.size() == 1)
    {
        for(::fwData::Object::sptr obj :  *selection)
        {
            ::fwMedData::ActivitySeries::sptr as = ::fwMedData::ActivitySeries::dynamicCast(obj);
            if (!as)
            {
                launchAS = false;
                break;
            }
            else
            {
                this->launchActivitySeries(as);
                launchAS = true;
            }
        }
    }
    return launchAS;
}

//------------------------------------------------------------------------------

void SActivityLauncher::launchSeries(::fwMedData::Series::sptr series)
{
    ::fwMedData::ActivitySeries::sptr as = ::fwMedData::ActivitySeries::dynamicCast(series);
    if (as)
    {
        this->launchActivitySeries(as);
    }
    else
    {
        ::fwData::Vector::sptr selection = ::fwData::Vector::New();
        selection->getContainer().push_back(series);
        ActivityInfoContainer infos = ::fwActivities::registry::Activities::getDefault()->getInfos(selection);

        if( m_quickLaunch.find( series->getClassname() ) != m_quickLaunch.end() )
        {
            std::string activityId = m_quickLaunch[ series->getClassname() ];
            SLM_ASSERT("Activity information not found for" + activityId,
                       ::fwActivities::registry::Activities::getDefault()->hasInfo(activityId) );
            this->sendConfig( ::fwActivities::registry::Activities::getDefault()->getInfo(activityId) );
        }
        else if ( !infos.empty() )
        {
            this->sendConfig( infos.front() );
        }
        else
        {
            ::fwGui::dialog::MessageDialog::show("Activity launcher",
                                                 "Not available activity for the current selection.",
                                                 ::fwGui::dialog::MessageDialog::WARNING);
        }
    }
}

//------------------------------------------------------------------------------

void SActivityLauncher::launchActivitySeries(::fwMedData::ActivitySeries::sptr series)
{
    ::fwActivities::registry::ActivityInfo info;
    info = ::fwActivities::registry::Activities::getDefault()->getInfo(series->getActivityConfigId());

    // Applies activity validator on activity series to check the data
    if (!info.validatorsImpl.empty())
    {
        for (std::string validatorImpl : info.validatorsImpl)
        {
            /// Process activity validator
            ::fwActivities::IValidator::sptr validator = ::fwActivities::validator::factory::New(validatorImpl);

            ::fwActivities::IActivityValidator::sptr activityValidator =
                ::fwActivities::IActivityValidator::dynamicCast(validator);

            if (activityValidator)
            {
                ::fwActivities::IValidator::ValidationType validation = activityValidator->validate(series);
                if(!validation.first)
                {
                    std::string message = "The activity '" + info.title + "' can not be launched:\n" +
                                          validation.second;
                    ::fwGui::dialog::MessageDialog::show("Activity launch",
                                                         message,
                                                         ::fwGui::dialog::IMessageDialog::CRITICAL);
                    return;
                }
            }
        }
    }

    ParametersType parameters = this->translateParameters(m_parameters);
    ::fwActivities::registry::ActivityMsg msg = ::fwActivities::registry::ActivityMsg(series, info, parameters);

    m_sigActivityLaunched->asyncEmit(msg);
}

//------------------------------------------------------------------------------

SActivityLauncher::ParametersType SActivityLauncher::translateParameters( const ParametersType& parameters )
{
    ParametersType transParams = parameters;
    ::fwData::Object::csptr workingObj = this->getInput< ::fwData::Object >(s_SERIES_INPUT);
    SLM_ASSERT("The input key '" + s_SERIES_INPUT + "' is not correctly set.", workingObj);

    for(ParametersType::value_type& param :  transParams)
    {
        if(param.isSeshat())
        {
            std::string parameterToReplace = param.by;
            if (parameterToReplace.substr(0, 1) == "!")
            {
                parameterToReplace.replace(0, 1, "@");
            }

            ::fwData::Object::sptr obj = ::fwDataCamp::getObject(workingObj, parameterToReplace);
            SLM_ASSERT("Invalid seshat path : '"<<param.by<<"'", obj);

            ::fwData::String::sptr stringParameter = ::fwData::String::dynamicCast(obj);

            std::string parameterValue = obj->getID();

            if(stringParameter && param.by.substr(0, 1) == "!")
            {
                parameterValue = stringParameter->getValue();
            }
            param.by = parameterValue;
        }
    }
    return transParams;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SActivityLauncher::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_SERIES_INPUT, ::fwData::Vector::s_ADDED_OBJECTS_SIG, s_UPDATE_STATE_SLOT );
    connections.push(s_SERIES_INPUT, ::fwData::Vector::s_REMOVED_OBJECTS_SIG, s_UPDATE_STATE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

}
}

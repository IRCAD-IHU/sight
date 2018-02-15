/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "activities/config.hpp"

#include <fwActivities/registry/Activities.hpp>
#include <fwActivities/registry/ActivityMsg.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <QButtonGroup>
#include <QObject>
#include <QPointer>

namespace activities
{
namespace editor
{

/**
 * @brief This editor launchs an activity according to the given configuration
 *
 * This editor proposes all the available activities according to the given configuration.
 * It sends a signal with the activity identifier when a button is pushed.
 *
 * It should work with the ::uiMedDataQt::editor::SActivityWizard that creates or updates the activitySeries.
 *
 * @section Signals Signals
 * - \b activityIDSelected(std::string) : This signal is emitted when the activity is selected, it
 *      contains all activity identifier. It should be connected to the slot 'createActivity' of the service
 *     'SActivityWizard'.
 * - \b loadRequested() : This signal is emitted when the "load activity" button is pushed.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="editor_newActivity" type="::activities::editor::SCreateActivity" >
       <filter>
           <mode>include</mode>
           <id>2DVisualizationActivity</id>
           <id>3DVisualizationActivity</id>
           <id>VolumeRenderingActivity</id>
       </filter>
   </service>
   @endcode
 *
 * - \b filter (optional): it allows to filter the activity that can be proposed.
 *    - \b mode: 'include' or 'exclude'. Defines if the activity in the following list are proposed (include) or not
 *      (exclude).
 *    - \b id: id of the activity
 */
class ACTIVITIES_CLASS_API SCreateActivity : public QObject,
                                             public ::fwGui::editor::IEditor
{
Q_OBJECT;

public:

    fwCoreServiceClassDefinitionsMacro( (SCreateActivity)(::fwGui::editor::IEditor) );

    /// Constructor. Do nothing.
    ACTIVITIES_API SCreateActivity() noexcept;

    /// Destructor. Do nothing.
    ACTIVITIES_API virtual ~SCreateActivity() noexcept;

    /**
     * @name Signals API
     * @{
     */
    ACTIVITIES_API static const ::fwCom::Signals::SignalKeyType s_ACTIVITY_ID_SELECTED_SIG;
    typedef ::fwCom::Signal< void (std::string) > ActivityIDSelectedSignalType;

    ACTIVITIES_API static const ::fwCom::Signals::SignalKeyType s_LOAD_REQUESTED_SIG;
    typedef ::fwCom::Signal< void () > LoadRequestedSignalType;
    /// @}

protected:

    /// Initialize the editor.
    virtual void configuring() override;

    /// This method launches the IEditor::starting method.
    virtual void starting() override;

    /// This method launches the IEditor::stopping method.
    virtual void stopping() override;

    /// Show activity selector.
    virtual void updating() override;

    typedef std::vector< std::string > KeysType;

private Q_SLOTS:

    void onClicked(int id);

private:

    /**
     * @brief Slots to launch the given activitySeries.
     * @param activitySeries the activity is launched on this series.
     */
    void launchActivity(::fwMedData::ActivitySeries::sptr activitySeries);

    typedef ::fwActivities::registry::Activities::ActivitiesType ActivityInfoContainer;

    /// Returns enabled activity infos according to activity filter.
    ActivityInfoContainer getEnabledActivities(const ActivityInfoContainer& infos);

    /**
     * @brief Filter mode : include or exclude activity configurations.
     * @note Allowed values : 'include' or 'exclude'
     */
    std::string m_filterMode;

    /// Id-s of activity configurations to be enabled or disabled, according to filter mode.
    KeysType m_keys;

    /// Informations used to launch activities
    ActivityInfoContainer m_activitiesInfo;

    /// Pointer on the buttons group
    QPointer<QButtonGroup> m_buttonGroup;
};

} // namespace editor
} // namespace activities

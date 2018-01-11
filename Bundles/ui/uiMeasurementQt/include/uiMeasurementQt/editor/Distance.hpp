/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENTQT_EDITOR_DISTANCE_HPP__
#define __UIMEASUREMENTQT_EDITOR_DISTANCE_HPP__

#include "uiMeasurementQt/config.hpp"

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>
#include <QPushButton>

namespace uiMeasurementQt
{
namespace editor
{
/**
 * @brief   Distance service is represented by a button. It allows to show distances in a generic scene.
 *
 * @note Sends a signal to request a distance. It should be conected to an ImageMultiDistance adaptor to create the
 * distance.
 *
 * @section Signals Signals
 *  \b distanceRequested() : signal to request a distance. It should be conected to an ImageMultiDistance
 * adaptor to create the distance.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="::uiMeasurementQt::editor::Distance" >
       <inout key="image" uid="..." />
       <placeInScene uid="genericScene" />
   </service>
   @endcode

 * @subsection In-Out In-Out
 * -\b image [::fwData::Image]: Image in which we calculate the distance.
 * @subsection Configuration Configuration
   \b genericScene is the uid of the ::fwRenderVTK::SRender representing the generic scene which will be printed.
 */
class UIMEASUREMENTQT_CLASS_API Distance : public QObject,
                                           public ::gui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro( (Distance)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIMEASUREMENTQT_API Distance() noexcept;

    /// Destructor. Do nothing.
    UIMEASUREMENTQT_API virtual ~Distance() noexcept;

    /**
     * @name Signal
     * @{
     */
    UIMEASUREMENTQT_API static const ::fwCom::Signals::SignalKeyType s_DISTANCE_REQUESTED_SIG;
    typedef ::fwCom::Signal< void () > DistanceRequestedSignalType;
    /**
     * @}
     */
protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Install the layout.
     *
     * This method launches the IEditor::starting method.
     */
    virtual void starting() override;

    /**
     * @brief Destroy the layout.
     *
     * This method launches the IEditor::stopping method.
     */
    virtual void stopping() override;

    /// Do nothing
    virtual void updating() override;

    /// Do nothing
    virtual void swapping() override;

    virtual void configuring() override;

    /// Overrides
    virtual void info( std::ostream& _sstream ) override;

protected Q_SLOTS:
    /**
     * @brief Notify the scene that a distance is added.
     */
    void onDistanceButton();

private:

    std::string m_scenesUID;
    QPointer< QPushButton > m_distButton;

    DistanceRequestedSignalType::sptr m_sigDistanceRequested; /// signal emitted when a distance is requested

};
} // namespace editor
} // uiMeasurementQt

#endif /*__UIMEASUREMENTQT_EDITOR_DISTANCE_HPP__*/


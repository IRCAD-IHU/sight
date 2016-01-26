/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_EDITOR_SSLIDER_HPP__
#define __VIDEOQT_EDITOR_SSLIDER_HPP__

#include <QObject>
#include <QSlider>
#include <QLabel>
#include <QPointer>

#include <fwTools/Failed.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <gui/editor/IEditor.hpp>

#include "videoQt/config.hpp"

namespace videoQt
{
namespace editor
{

/**
 * @brief   This editor allows to draw a slider. It is designed to be used with SFrameGrabber to browse a video.
 * @class   SSlider
 */
class VIDEOQT_CLASS_API SSlider : public QObject,
                                  public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (SSlider)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    VIDEOQT_API SSlider() throw();

    /// Destructor. Do nothing.
    VIDEOQT_API virtual ~SSlider() throw();

    /**@name Signals API
     * @{
     */

    VIDEOQT_API static const ::fwCom::Signals::SignalKeyType s_POSITION_CHANGED_SIG;
    typedef ::fwCom::Signal<void (int64_t)> PositionChangedSignalType;

    /** @} */

    /**
     * @name Slots API
     * @{
     */

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_SET_POSITION_SLIDER_SLOT;
    typedef ::fwCom::Slot<void (int64_t)> ChangePositionSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_SET_DURATION_SLIDER_SLOT;
    typedef ::fwCom::Slot<void (int64_t)> ChangeDurationSlotType;

    ///@}

protected:


    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Installs the layout
    virtual void starting() throw(::fwTools::Failed);

    /// Destroys the layout
    virtual void stopping() throw(::fwTools::Failed);

    /// Does nothing
    virtual void updating() throw(::fwTools::Failed);

    /// Does nothing
    virtual void swapping() throw(::fwTools::Failed);

    /**
     * @brief Configure the service
     *
     * @code{.xml}
        <service uid="..." type="::gui::editor::IEditor" impl="::videoQt::editor::SSlider" autoConnect="no"/>
       @endcode
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Signal when the position os the slider changed
    PositionChangedSignalType::sptr m_sigPositionChanged;

    /// SLOT : Call to set the video position.
    void setPosition(int64_t newPos);

    /// SLOT : Call to set the video position.
    void setDuration(int64_t duration);

protected Q_SLOTS:

    /// Calls when the cursor is moved.
    void changePosition();

    /// Calls when the cursor starts to move .
    void sliderPressed();

private:

    /// Slider to show progress.
    QPointer<QSlider> m_positionSlider;
    QPointer<QLabel> m_currentPosition;
    QPointer<QLabel> m_totalDuration;

    /// Is the slider pressed ?
    bool m_sliderPressed;
};

} // editor
} // videoQt

#endif /*__VIDEOQT_EDITOR_SSLIDER_HPP__*/



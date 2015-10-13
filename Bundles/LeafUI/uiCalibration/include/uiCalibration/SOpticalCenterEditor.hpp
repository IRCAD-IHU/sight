/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UICALIBRATION_SOPTICALCENTEREDITOR_HPP__
#define __UICALIBRATION_SOPTICALCENTEREDITOR_HPP__

#include "uiCalibration/config.hpp"

#include <gui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>
#include <QSlider>
#include <QLabel>

namespace uiCalibration
{

/**
 * @brief   This editor shows sliders to configure an intrinsic camera calibration.
 * @class   SOpticalCenterEditor
 */
class UICALIBRATION_CLASS_API SOpticalCenterEditor : public QObject,
                                                     public ::gui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro ( (SOpticalCenterEditor)(::gui::editor::IEditor) );

    /// Constructor.
    UICALIBRATION_API SOpticalCenterEditor() throw();

    /// Destructor. Does nothing
    UICALIBRATION_API virtual ~SOpticalCenterEditor() throw();

protected:

    /**
     * @brief method description:
     * @verbatim
        <service uid="SOpticalCenterEditorInstance" impl="::uiCalibration::SOpticalCenterEditor" type="::gui::editor::IEditor">
             <cameraSrcUid>...</cameraSrcUid>
        </service>
       @endverbatim
     * - \b cameraSrcUid (optional) : deepCopy source camera on starting if it exists.
     */
    UICALIBRATION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Create UI.
    UICALIBRATION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Destroy UI.
    UICALIBRATION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Does nothing.
    UICALIBRATION_API virtual void updating() throw ( ::fwTools::Failed );

private Q_SLOTS:

    void onCxSliderChanged(int value);
    void onCySliderChanged(int value);
    void onFySliderChanged(int value);

private:

    QPointer<QSlider> m_cxSlider; ///< Slider to modify cx
    QPointer<QSlider> m_cySlider; ///< Slider to modify cy
    QPointer<QSlider> m_fySlider; ///< Slider to modify fy
    QPointer<QLabel> m_cxLabel;   ///< Label for cx value
    QPointer<QLabel> m_cyLabel;   ///< Label for cy value
    QPointer<QLabel> m_fyLabel;   ///< Label for fy value

    std::string m_cameraSrcUid; ///< uid of the source camera
};

} // uiCalibration

#endif // __UICALIBRATION_SOPTICALCENTEREDITOR_HPP__

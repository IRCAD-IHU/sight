/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKARADAPTOR_SCAMERA_HPP__
#define __VISUVTKARADAPTOR_SCAMERA_HPP__

#include "visuVTKARAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>
#include <fwServices/helper/SigSlotConnection.hpp>


namespace fwData
{
class Image;
}

namespace arData
{
class Camera;
}

class vtkPerspectiveTransform;
class vtkCommand;

namespace visuVTKARAdaptor
{

/**
 * @brief   Adaptor to place the camera in the scene
 * @class   SCamera
 */
class VISUVTKARADAPTOR_CLASS_API SCamera : public ::fwRenderVTK::IVtkAdaptorService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (SCamera)(::fwRenderVTK::IVtkAdaptorService) );

    /// Constructor
    SCamera() throw();

    /// Destructor
    virtual ~SCamera() throw();

    /**
     * @name Signals API
     * @{
     */
    VISUVTKARADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_POSITION_MODIFIED_SIG;
    typedef ::fwCom::Signal<void ()> PositionModifiedSignalType;
    ///@}

    /**
     * @name Slots API
     * @{
     */
    VISUVTKARADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_CALIBRATE_SLOT;
    typedef ::fwCom::Slot<void ()> CalibrateSlotType;
    ///@}

    /// Update Camera position from VTK.
    VISUVTKARADAPTOR_API void updateFromVtk();

protected:

    /// Initializes the camera position and image connections
    VISUVTKARADAPTOR_API void doStart() throw(fwTools::Failed);

    /**
     * @code{.xml}
       <adaptor id="camera" class="::visuVTKARAdaptor::SCamera" objectId="transform">
        <config renderer="default" cameraUID="..." />
       </adaptor>
       @endcode
     * - \b renderer : defines the renderer to show the arrow. It must be different from the 3D objects renderer.
     * - \b cameraUID (optional): defines the uid of the camera (used to calibrate the vtk camera)
     */
    VISUVTKARADAPTOR_API void doConfigure() throw(fwTools::Failed);

    /// Calls doStop() and doStart()
    VISUVTKARADAPTOR_API void doSwap() throw(fwTools::Failed);

    /// Does nothing
    VISUVTKARADAPTOR_API void doUpdate() throw(fwTools::Failed);

    /// Removes camera observers and connections
    VISUVTKARADAPTOR_API void doStop() throw(fwTools::Failed);

private:

    /// Update vtk camera from current TransformationMatrix3D
    void updateFromTMatrix3D();

    /// Calibrate vtk camera
    void calibrate();

    std::string m_cameraUID; ///< uid of the camera

    SPTR(::arData::Camera) m_camera; ///< camera used to calibrate vtk camera

    vtkPerspectiveTransform* m_transOrig; ///<  VTK original perspective transform.
    vtkCommand* m_cameraCommand; ///< VTK camera command.

    ::fwServices::helper::SigSlotConnection::sptr m_connections; ///< Connection to image

    CalibrateSlotType::sptr m_slotCalibrate; ///< Slot to calibrate camera
};

} //namespace visuVTKARAdaptor

#endif /* __VISUVTKARADAPTOR_SCAMERA_HPP__ */

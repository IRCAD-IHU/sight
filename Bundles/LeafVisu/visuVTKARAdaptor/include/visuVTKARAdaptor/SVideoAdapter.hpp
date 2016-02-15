/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKARADAPTOR_SVIDEOADAPTER_HPP__
#define __VISUVTKARADAPTOR_SVIDEOADAPTER_HPP__

#include "visuVTKARAdaptor/config.hpp"

#include <fwRenderVTK/IVtkAdaptorService.hpp>

class vtkImageData;
class vtkTexture;
class vtkActor;

namespace arData
{
class Camera;
}

namespace visuVTKARAdaptor
{

/**
 * @brief   Adaptor to render a video frame from a 2D-image.
 * @class   SVideoAdapter
 */
class VISUVTKARADAPTOR_CLASS_API SVideoAdapter : public ::fwRenderVTK::IVtkAdaptorService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (SVideoAdapter)(::fwRenderVTK::IVtkAdaptorService) );

    /// Constructor
    SVideoAdapter() throw();

    /// Destructor
    virtual ~SVideoAdapter() throw();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_IMAGE_SLOT
     * Connect Image::s_VISIBILITY_MODIFIED_SIG to this::s_UPDATE_IMAGE_OPACITY_SLOT
     * Connect Image::s_TRANSPARENCY_MODIFIED_SIG to this::s_UPDATE_IMAGE_OPACITY_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKARADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    /// Create the actor and mapper used to show the video frame.
    VISUVTKARADAPTOR_API void doStart() throw(fwTools::Failed);

    /**
     * @code{.xml}
       <adaptor id="video" class="::visuVTKARAdaptor::SVideoAdapter" objectId="imageKey">
        <config renderer="default" cameraUID="..." reverse="true" />
       </adaptor>
       @endcode
     * - \b renderer : defines the renderer to show the arrow. It must be different from the 3D objects renderer.
     * - \b cameraUID (optional) : defines the uid of the camera used to place video plane.
     * - \b reverse (optional)(default: true) : if true, the actor is rotated in z and y axis.
     */
    VISUVTKARADAPTOR_API void doConfigure() throw(fwTools::Failed);

    /// Calls doUpdate()
    VISUVTKARADAPTOR_API void doSwap() throw(fwTools::Failed);

    /// Updated the frame from the current Image.
    VISUVTKARADAPTOR_API void doUpdate() throw(fwTools::Failed);

    /// Removes the actor from the renderer
    VISUVTKARADAPTOR_API void doStop() throw(fwTools::Failed);

private:

    /// Slot: Update image opacity and visibility
    void updateImageOpacity();

    /// Slot: update image
    void updateImage();

    /// Slot: apply the optical center offset to our video plane
    void offsetOpticalCenter();

    vtkImageData* m_imageData; ///< vtk image created from current data Image. It is shown in the frame.
    vtkTexture* m_texture;  ///< texture used to show the image
    vtkActor * m_actor;  ///< actor to show frame

    bool m_isTextureInit; /// true if the texture is initialized

    std::string m_cameraUID; ///< uid of the camera

    bool m_reverse; ///< if true, the actor is rotated in z and y axis.

    SPTR(::arData::Camera) m_camera; ///< camera used to retrieve the optical center
};

} //namespace visuVTKARAdaptor

#endif /* __VISUVTKARADAPTOR_SVIDEOADAPTER_HPP__ */

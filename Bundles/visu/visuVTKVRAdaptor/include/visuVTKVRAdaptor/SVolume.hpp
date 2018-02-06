/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuVTKVRAdaptor/config.hpp"

#include <fwData/Image.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>
#include <fwRenderVTK/SRender.hpp>

class vtkSmartVolumeMapper;
class vtkBoxWidget2;
class vtkColorTransferFunction;
class vtkCommand;
class vtkPiecewiseFunction;
class vtkPlaneCollection;
class vtkVolume;
class vtkVolumeProperty;

namespace visuVTKVRAdaptor
{

/**
 * @brief This adaptor displays a volume image.
 *
 * @section Slots Slots
 * - \b resetBoxWidget() : reset the clipping box widget around the volume.
 * - \b activateBoxClipping(bool) : show/hide clipping box.
 * - \b show(bool) : show/hide the volume.
 * - \b updateTFPoints() : updates the volume transfer function according to the new points
 * - \b updateTFWindowing(double window, double level) : updates the volume transfer function according to the new
 *      window and level
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::visuVTKVRAdaptor::SVolume">
            <inout key="image" uid="..." />
            <inout key="tf" uid="..." optional="yes" />
            <config renderer="default"  clippingplanes="clippingPlanesId" autoresetcamera="yes|no" croppingBox="yes|no"
                    reductionFactor="0.5" cropBoxTransform="cropTransform" transform="trf" blend="composite"/>
       </service
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 * - \b tf [::fwData::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel). The transferFunction's signals are automatically connected to
 *      the slots 'updateTFPoints' and 'updateTFWindowing'.
 *
 * @subsection Configuration Configuration
 * - \b renderer : ID of renderer the adaptor must use
 * - \b clippingplanes (optional) : id of VTK object for clipping planes
 * - \b autoresetcamera (optional, default: yes) : defines if the renderer must reset camera when updating image
 * - \b croppingBox (optional, default: no) : defines if the cropping box must be shown
 * - \b reductionFactor (optional, [0-1]) : factor to resample the original image.
 * - \b cropBoxTransform (optional) : vtkTransform applied to the cropping box.
 * - \b transform (optional) : vtkTransform applied to the volume.
 * - \b blend (optional) : vtk BlendMode used for rendering: composite, additive, min, max, average (default:
 *      composite). Warning : average mode will use GPU .
 */
class VISUVTKVRADAPTOR_CLASS_API SVolume : public ::fwDataTools::helper::MedicalImageAdaptor,
                                           public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SVolume)(::fwRenderVTK::IAdaptor) );

    VISUVTKVRADAPTOR_API SVolume() noexcept;

    VISUVTKVRADAPTOR_API virtual ~SVolume() noexcept;

    static const ::fwServices::IService::KeyType s_IMAGE_INOUT;
    static const ::fwServices::IService::KeyType s_TF_INOUT;

    VISUVTKVRADAPTOR_API void setClippingPlanesId( ::fwRenderVTK::SRender::VtkObjectIdType id );

    VISUVTKVRADAPTOR_API void setVtkClippingPlanes( vtkPlaneCollection* planes );

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKVRADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Apply the cropping on volume rendering
    VISUVTKVRADAPTOR_API void crop();

    /// Update associated transform adaptor with CropBox transform
    VISUVTKVRADAPTOR_API void updateTransform();

    /// Update CropBox transform with transform adaptor
    VISUVTKVRADAPTOR_API void updateCropBoxTransform();

protected:

    VISUVTKVRADAPTOR_API void configuring() override;
    VISUVTKVRADAPTOR_API void starting() override;
    VISUVTKVRADAPTOR_API void stopping() override;
    VISUVTKVRADAPTOR_API void updating() override;
    VISUVTKVRADAPTOR_API void swapping(const KeyType& key) override;

    /// Slot: updates the volume transfer function
    VISUVTKVRADAPTOR_API virtual void updateTFPoints() override;

    /// Slot: updates the volume transfer function
    VISUVTKVRADAPTOR_API virtual void updateTFWindowing(double window, double level) override;

    /// Slot: reset the clipping box widget around the volume
    void resetBoxWidget();

    /// Slot: show/hide clipping box
    void activateBoxClipping( bool activate );

    /// Slot: show/hide the volume
    void show( bool isVisible );

    void buildPipeline();

    void updateImage( ::fwData::Image::sptr image  );

    void updateVolumeTransferFunction( ::fwData::Image::sptr image );

    vtkPlaneCollection* m_clippingPlanes;

    ::fwRenderVTK::SRender::VtkObjectIdType m_clippingPlanesId;

    vtkSmartVolumeMapper* m_volumeMapper;
    vtkVolumeProperty* m_volumeProperty;
    vtkVolume* m_volume;

    vtkPiecewiseFunction* m_opacityTransferFunction;
    vtkColorTransferFunction* m_colorTransferFunction;

    vtkCommand* m_abortCommand;

    vtkBoxWidget2* m_boxWidget;
    vtkCommand* m_croppingCommand;
    vtkCommand* m_transformCommand;

    /// Cropping box default state
    bool m_croppingBoxDefaultState;

    /// XML ID for the transformation matrix affected only to the cropBox.
    std::string m_cropBoxTransformID;

    /// Using the blend mode affects the VTKMapper class used for the rendering.
    std::string m_blendMode;

    /// Transformation matrix affected only to the cropBox.
    vtkTransform* m_cropBoxTransform;

private:

    bool m_autoResetCamera;

    double m_reductionFactor;
};

} //namespace visuVTKVRAdaptor

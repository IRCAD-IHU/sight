/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SQNegatoMPR_HPP__
#define __VISUVTKADAPTOR_SQNegatoMPR_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/HasSignals.hpp>
#include <fwCom/HasSlots.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IQAdaptor.hpp>
#include <fwRenderVTK/IAdaptor.hpp>

#include <fwServices/QtObjectHolder.hpp>

#include <vtkOutlineFilter.h>
#include <vtkImagePlaneWidget.h>

#include <boost/logic/tribool.hpp>

#include <QVariantMap>

namespace visuVTKAdaptor
{

class SliceCursor;

/**
 * @brief Display a negato image
 *
 * @section Slots Slots
 * - \b updateSliceType(int from, int to): update image slice type
 * - \b updateSliceMode(int mode): update Slice mode (0: NO_SLICE, 1: ONE_SLICE, 3: THREE_SLICES)
 * - \b showSlice(bool isShown): show/hide slice
 * - \b setCrossScale(double scale): set the slice cross scale. Forward the information to SliceCursor sub-adaptor.
 * - \b changeImageSource(std::string _value, std::string _key): set the VTK source image. The key must be "ImageSource"
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::visuVTKAdaptor::SQNegatoMPR" autoConnect="yes">
       <inout key="image" uid="..." />
       <inout key="tf" uid="..." optional="yes" />
       <config renderer="default" picker="negatodefault" mode="2d" slices="1" sliceIndex="axial"
               transform="trf" tfalpha="yes" interpolation="off" vtkimagesource="imgSource" actorOpacity="1.0" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 * - \b tf [::fwData::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the image.
 *    - \b picker (mandatory): identifier of the picker
 *    - \b mode (optional, 2d or 3d): defines the scene mode. In 2d mode, the camera follow the negato in
 *    axial/frontal/sagital orientation. In 3d mode, the camera is automatically reset when the image is modified. If
 *    mode is not defined, the camera is free.
 *    - \b slices (optional, default=3): number of slices shown in the adaptor
 *    - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
 *    - \b transform (optional): the vtkTransform to associate to the adaptor
 *    - \b tfalpha (optional, yes/no, default=no): if true, the opacity of the transfer function is used in the negato.
 *    - \b interpolation (optional, yes/no, default=yes): if true, the image pixels are interpolated
 *    - \b vtkimagesource (optional): source image, used for blend
 *    - \b actorOpacity (optional, default=1.0): actor opacity (float)
 */
class VISUVTKADAPTOR_CLASS_API SQNegatoMPR : public ::fwRenderVTK::IQAdaptor,
                                             public ::fwDataTools::helper::MedicalImageAdaptor
{
    Q_OBJECT
    Q_PROPERTY(fwServices::QtObjectHolder *image MEMBER m_image)


public:
    VISUVTKADAPTOR_API SQNegatoMPR() noexcept;

    VISUVTKADAPTOR_API virtual ~SQNegatoMPR() noexcept;

    static const ::fwServices::IService::KeyType s_IMAGE_INOUT;
    static const ::fwServices::IService::KeyType s_TF_INOUT;

    typedef enum
    {
        NO_SLICE = 0,
        ONE_SLICE,
        THREE_SLICES
    } SliceMode;

    //------------------------------------------------------------------------------

    void setAllowAlphaInTF(bool allow)
    {
        m_allowAlphaInTF = allow;
    }
    //------------------------------------------------------------------------------

    void setInterpolation(bool interpolation)
    {
        m_interpolation = interpolation;
    }
    //------------------------------------------------------------------------------

    void setVtkImageSourceId(std::string id)
    {
        m_imageSourceId = id;
    }

    VISUVTKADAPTOR_API void setSliceMode(SliceMode sliceMode);
    VISUVTKADAPTOR_API SliceMode getSliceMode() const;
    VISUVTKADAPTOR_API ::boost::logic::tribool is3dModeEnabled() const;
    VISUVTKADAPTOR_API void set3dMode( bool enabled );

    /// Set actor opacity
    void setActorOpacity(double actorOpacity)
    {
        m_actorOpacity = actorOpacity;
    }

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;
    VISUVTKADAPTOR_API void swapping(const KeyType& key);

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     */

    std::shared_ptr<::fwRenderVTK::IQAdaptor> addAdaptor(const std::string& adaptor, int axis = -1);

private:

    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice type
    void updateSliceType(int from, int to);

    /// Slot: update Slice mode (0: NO_SLICE, 1: ONE_SLICE, 3: THREE_SLICES)
    void updateSliceMode(int mode);

    /// Slot: show/hide slice
    void showSlice(bool isShown);

    /// Slot: set the slice cross scale. Forward the information to SliceCursor sub-adaptor.
    void setCrossScale(double scale);

    /// Slot: set the VTK source image. The key must be "ImageSource"
    void changeImageSource(std::string _value, std::string _key);

    /**
     * @}
     */

    bool m_allowAlphaInTF;
    bool m_interpolation;
    double m_actorOpacity;

    std::string m_imageSourceId;

    std::string m_slicingStartingProxy; ///< channel of the proxy used to start slicing
    std::string m_slicingStoppingProxy; ///< channel of the proxy used to stop slicing

    ::boost::logic::tribool m_3dModeEnabled;
    SliceMode m_sliceMode;
    SliceMode m_backupedSliceMode;
    ::fwCom::helper::SigSlotConnection m_connections; /// store subservices connections

    ::fwRenderVTK::IAdaptor::sptr m_sliceCursor;

    /**
     * @{
     */
    ::fwServices::QtObjectHolder  *m_image;
    /**
     * }@
     */
    /// @brief image wireframe outline.
    vtkOutlineFilter *m_outline;

    /// @brief vtk widget used in the negatoscope axial plane.
    vtkImagePlaneWidget *m_negatoAxial;

    /// @brief vtk widget used in the negatoscope sagittal plane.
    vtkImagePlaneWidget *m_negatoSagittal;

    /// @brief vtk widget used in the negatoscope frontal plane.
    vtkImagePlaneWidget *m_negatoFrontal;

};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SQNegatoMPR_HPP__

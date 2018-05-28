/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#ifndef __VISUVTKADAPTOR_SQMesh_HPP__
#define __VISUVTKADAPTOR_SQMesh_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwRenderVTK/IQAdaptor.hpp>

class vtkCommand;
class vtkDepthSortPolyData;
class vtkAlgorithm;
class vtkAlgorithmOutput;
class vtkPolyDataNormals;
class vtkPlaneCollection;
class vtkActorCollection;
class vtkActor;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkTransform;

namespace fwData
{
class Mesh;
class Material;
}

namespace visuVTKAdaptor
{

class SQTransform;

class VISUVTKADAPTOR_CLASS_API MeshVtkCommand;

/**
 * @brief   Display a ::fwData::Mesh in the generic scene
 *
 * @section Signals Signals
 * - \b textureApplied(SPTR(::fwData::Material)) : emitted when a texture was applied.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool) : change the visibility of the mesh.
 * - \b updatePointColors() : the mesh points colors were modified.
 * - \b updateCellColors() : the mesh cells colors were modified.
 * - \b updatePointNormals() : the mesh points normals were modified.
 * - \b updateCellNormals() : the mesh cells normals were modified.
 * - \b updatePointTexCoords() : the mesh points texture coordinates were modified.
 * - \b updateCellTexCoords() : the mesh cells texture coordinates were modified.
 * - \b showPointColors() : display the points colors (instead of cell colors).
 * - \b showCellColors() : display the cells colors (instead of point colors).
 * - \b hideColors() : hide the cells colors.
 * - \b updateColorMode(std::uint8_t) : update the color mode (0 : hide, 1 : points colors, 2 : cells colors).
 * - \b updateNormalMode(std::uint8_t) : updates the normal mode (0: hidden, 1: point normals, 2: cell normals)
 * - \b updateMatrixField(::fwData::Object::FieldsContainerType) : updates the matrix service if the
 *      'TransformMatrix' field changed

 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SQMesh"  autoConnect="yes">
       <in key="mesh" uid="..." />
       <config renderer="default" transform="..." picker="" texture="TextureAdaptor" />
   </service>
   @endcode
 * @subsection Input Input
 * - \b mesh [::fwData::Mesh]: mesh to display.
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer(mandatory) : renderer where the mesh is displayed
 *    - \b picker(optional) : picker used to pick on the mesh
 *    - \b transform(optional) : transform visually applied on the mesh
 *    - \b color : hexadecimal color code of the diffuse material (i.e RGB "#ff00ff" or RGBA "#ff224508"). This
 *    overrides the current diffuse color of the mesh material.
 *    - \b unclippedcolor : hexadecimal color code of the diffuse material of the unclipped part.
 *    - \b autoresetcamera : reset the camera point of view when the mesh is modified ("yes" or "no", default: "yes") .
 *    - \b uvgen : generate UV coordinates ("sphere", "cylinder" or "plane").
 *    - \b shadingMode : "ambient", "flat", "gouraud" or "phong" (default: phong).
 *    - \b clippingplane(optional) : identifier of the planes collection used for clipping
 */
class VISUVTKADAPTOR_CLASS_API SQMesh : public ::fwRenderVTK::IQAdaptor
{
Q_OBJECT
Q_PROPERTY(fwServices::QtObjectHolder* mesh MEMBER m_mesh)

public:
    VISUVTKADAPTOR_API SQMesh() noexcept;
    VISUVTKADAPTOR_API virtual ~SQMesh() noexcept;

    static const ::fwServices::IService::KeyType s_MESH_INPUT;

    /**
     * @name Signals API
     * @{
     */
    typedef ::fwCom::Signal< void ( SPTR(::fwData::Material) ) > TextureAppliedSignalType;
    VISUVTKADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_TEXTURE_APPLIED_SIG;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_POINT_COLORS_SLOT;
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CELL_COLORS_SLOT;
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VERTEX_SLOT;
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_POINT_NORMALS_SLOT;
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CELL_NORMALS_SLOT;
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_POINT_TEX_COORDS_SLOT;
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CELL_TEX_COORDS_SLOT;
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SHOW_POINT_COLORS_SLOT;
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SHOW_CELL_COLORS_SLOT;
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_HIDE_COLORS_SLOT;
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_COLOR_MODE_SLOT;
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_NORMAL_MODE_SLOT;
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_MATRIX_FIELD_SLOT;

    /// Slot: update mesh visibility (true = visible)
    VISUVTKADAPTOR_API void updateVisibility ( bool isVisible );
    /** @} */

    VISUVTKADAPTOR_API SPTR(::fwData::Material) getMaterial() const;
    VISUVTKADAPTOR_API void setMaterial(SPTR(::fwData::Material) material);

    VISUVTKADAPTOR_API SPTR(::fwData::Material) getUnclippedMaterial() const;
    VISUVTKADAPTOR_API void setUnclippedPartMaterial(SPTR(::fwData::Material) material);

    VISUVTKADAPTOR_API void setShowClippedPart ( bool show );
    VISUVTKADAPTOR_API void setClippingPlanesId( ::fwRenderVTK::SQRender::VtkObjectIdType id );

    VISUVTKADAPTOR_API void setVtkClippingPlanes               ( vtkPlaneCollection* planes );
    VISUVTKADAPTOR_API void setActorPropertyToUnclippedMaterial( bool opt );

    /// Active/Inactive automatic reset on camera. By default =true.
    VISUVTKADAPTOR_API void setAutoResetCamera(bool autoResetCamera);

    VISUVTKADAPTOR_API bool getVisibility() const;

    /// Creates normals adaptor if material has NORMALS, else remove normals adaptor.
    VISUVTKADAPTOR_API void updateOptionsMode();

    /// Used to update normal mode (0: none, 1: point, 2: cell)
    VISUVTKADAPTOR_API void updateNormalMode(std::uint8_t mode);

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @name Slots methods
     * @{
     */

    /// Slot:  used to update point colors
    void updatePointColors();

    /// Slot: used to update cell colors
    void updateCellColors();

    /// Slot: used to update mesh vertex
    void updateVertex();

    ///Slot: used to update point normals
    void updatePointNormals();

    /// Slot: used to update cell normals
    void updateCellNormals();

    /// Slot: used to update point tex coords
    void updatePointTexCoords();

    /// Slot: used to update cell tex coords
    void updateCellTexCoords();

    /// Slot: used to show point colors
    void showPointColors();

    /// Slot: used to show cell colors
    void showCellColors();

    /// Slot: used to hide colors
    void hideColors();

    /// Slot: used to update color mode (0: none, 1: point, 2: cell)
    void updateColorMode(std::uint8_t mode);

    /// Slot: update the matrix service if the 'TransformMatrix' field changed
    void updateMatrixField(::fwData::Object::FieldsContainerType fields);
    /**
     * @}
     */

    vtkActor* newActor();
    void buildPipeline();

    void updateMesh( CSPTR(::fwData::Mesh) mesh );

    void setServiceOnMaterial(std::shared_ptr<::fwRenderVTK::IQAdaptor> &srv,
                              SPTR(::fwData::Material) material);

    void removeServicesStarterCommand();
    void createServicesStarterCommand();

    void createTransformService();
    void createNormalsService();
    void removeNormalsService();

    bool m_showClippedPart;
    bool m_autoResetCamera;

    vtkPolyData* m_polyData;
    vtkPolyDataMapper* m_mapper;
    vtkActor* m_actor;

    vtkPlaneCollection* m_clippingPlanes;
    ::fwRenderVTK::SQRender::VtkObjectIdType m_clippingPlanesId;

    MeshVtkCommand* m_servicesStarterCallback;

    SPTR(::fwData::Material)       m_material;
    SPTR(::fwData::Material)       m_unclippedPartMaterial;

    WPTR(::fwRenderVTK::IQAdaptor) m_materialService;
    WPTR(::fwRenderVTK::IQAdaptor) m_unclippedPartMaterialService;
    std::weak_ptr<::fwRenderVTK::IQAdaptor> m_normalsService;

    vtkTransform* m_transform;
    WPTR(::visuVTKAdaptor::SQTransform) m_transformService;

    /**
     * @brief Type for generation of texture coordinates
     */
    enum UvGenType
    {
        NONE,
        SPHERE,
        CYLINDER,
        PLANE,
    };

    UvGenType m_uvgen;

    /// Signal to emit when a texture must be applied on a material.
    TextureAppliedSignalType::sptr m_sigTextureApplied;

Q_SIGNALS:
    void    textureApplied(QObject* material);

private:

    ::fwServices::QtObjectHolder* m_mesh = nullptr;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SQMesh_HPP__

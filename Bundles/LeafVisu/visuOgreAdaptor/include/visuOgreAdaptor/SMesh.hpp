/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SMESH_HPP__
#define __VISUOGREADAPTOR_SMESH_HPP__

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Slot.hpp>

#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>

#include <Ogre.h>

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"
#include "visuOgreAdaptor/STransform.hpp"


fwCorePredeclare( (fwData)(Material) )
fwCorePredeclare( (fwData)(Mesh) )
fwCorePredeclare( (fwRenderOgre)(R2VBRenderable) )

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor shows individual meshes.
 *
 * This class handles the conversion of ::fwData::Mesh to Ogre3d. It can handle triangles, edges, quads or tetrahedrons.
 * For the quads and tetrahedrons, we generate the triangles in a pre-process, using the render to vertex buffer (r2vb)
 * feature to avoid the cost of geometry shaders when usign multi-pass rendering techniques.
 *
 * An Ogre entity is created from the mesh. A second mesh and a second entity are created as an input for the r2vb.
 * Thus, the main mesh will contains only triangles or edges, while the second entity contains only quads or tetras.
 * An Ogre material is also created, and then managed by a SMaterial adaptor (specified in the configuration otherwise
 * a new one is generated).
 *
 * To handle the per-primitive color of ::fwData::Mesh we also rely on geometry shaders, and thus on r2vb. We build a
 * texture containing the color for each primitive. This texture is fetched inside the geometry shader using the
 * primitive id.
 *
 * @class SMesh
 */
class VISUOGREADAPTOR_CLASS_API SMesh : public ::fwRenderOgre::IAdaptor,
                                        public ::fwRenderOgre::ITransformable
{
public:
    fwCoreServiceClassDefinitionsMacro((SMesh)(::fwRenderOgre::IAdaptor));

    /// Constructor: Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SMesh() throw();
    /// Destructor: if an entity exists in the Ogre Scene, asks Ogre to destroy it.
    VISUOGREADAPTOR_API virtual ~SMesh() throw();

    /// Returns the material associated to this.
    VISUOGREADAPTOR_API SPTR(::fwData::Material) getMaterial() const;
    /// Sets the current material.
    VISUOGREADAPTOR_API void setMaterial(SPTR(::fwData::Material) material);
    /// Sets the material template Name.
    VISUOGREADAPTOR_API void setMaterialTemplateName(const std::string& materialName);

    /// Active/Inactive automatic reset on camera. By default =true.
    VISUOGREADAPTOR_API void setAutoResetCamera(bool autoResetCamera);

    /// Returns associated entity
    VISUOGREADAPTOR_API ::Ogre::Entity* getEntity() const;

    /// Returns if the SMesh is visible in the scene or not.
    VISUOGREADAPTOR_API bool getVisibility() const;
    /// Sets whether the mesh is to be seen or not.
    VISUOGREADAPTOR_API void updateVisibility(bool isVisible);

    /// Set meshes vertex buffer to dynamic state (only has effect if called before service starting/update)
    VISUOGREADAPTOR_API void setDynamicVertices(bool _isDynamic);
    /// Set meshes and indices buffers to dynamic state (only has effect if called before service starting/update)
    VISUOGREADAPTOR_API void setDynamic(bool _isDynamic);

    VISUOGREADAPTOR_API void updateOptionsMode();

    /// This method is called by a reconstruction adaptor after creating the mesh adaptor
    VISUOGREADAPTOR_API void setIsReconstructionManaged(bool _isReconstructionManaged);

    /// Returns proposals to connect service slots to associated object signals
    ::fwServices::IService::KeyConnectionsType getObjSrvConnections() const;

    /// Ask the render service (SRender) to update - we also flag the r2vb objects as dirty
    FWRENDEROGRE_API virtual void requestRender();

private:

    enum BufferBinding
    {
        POSITION_NORMAL = 0,
        COLOUR          = 1,
        TEXCOORD        = 2,
        NUM_BINDINGS
    };

    /**
     * @brief Configures the adaptor
     * @verbatim
       <adaptor id="meshAdaptor" class="::visuOgreAdaptor::SMesh" objectId="meshKey">
        <config renderer="rendererId" transform="transformUID" materialAdaptor="materialName" textureAdaptor="texAdaptorUID" />
       </adaptor>
       @endverbatim
     * With :
     *  - \b renderer (mandatory) : defines the mesh's layer
     *  - \b transform (optional) : the transformation matrix to associate to the adaptor
     * Either of the following (whether a material is configured in the XML scene or not) :
     *  - \b materialAdaptor (optional) : the name of the associated material adaptor
     *  - \b materialTemplate (optional) : the name of the base Ogre material for the created SMaterial
     * Only if there is no material adaptor configured in the XML scene (in this case, it has to retrieve the texture
     * adaptor) :
     *  - \b textureAdaptor (optional) : the texture adaptor that the material will be listening to
     */
    void doConfigure() throw(fwTools::Failed);
    /// Manually creates a Mesh in the Default Ogre Ressource group
    void doStart    () throw(fwTools::Failed);
    /// Deletes the mesh after unregistering the service, and shutting connections.
    void doStop     () throw(fwTools::Failed);
    /// Performs a Stop -> Start
    void doSwap     () throw(fwTools::Failed);
    /// Checks if the fwData::Mesh has changed, and updates it if it has.
    void doUpdate   () throw(fwTools::Failed);

    ::Ogre::Entity* newEntity();

    /// Updates the Mesh, checks if color, number of vertices have changed, and updates them.
    void updateMesh(const ::fwData::Mesh::sptr& mesh);
    /// Updates the vertices position
    void updateVertices(const ::fwData::Mesh::sptr& mesh);
    /// Updates the vertices colors.
    void updateColors(const ::fwData::Mesh::sptr& mesh);
    /// Updates the vertices texture coordinates.
    void updateTexCoords(const ::fwData::Mesh::sptr& mesh);
    /// Erase the mesh data, called when the configuration change (new layer, etc...), to simplify modifications.
    void clearMesh(const ::fwData::Mesh::sptr& mesh);

    /// Associates a new SMaterial to the managed SMesh.
    /// With this method, SMesh is responsible for creating a SMaterial
    void updateNewMaterialAdaptor();
    /// Updates the associated material adaptor.
    /// This method is called when a material adaptor has been configured in the XML scene
    void updateXMLMaterialAdaptor();

    /// Creates a transform Service, and attaches it to a corresponding sceneNode in the Ogre scene.
    void createTransformService();

    /// Attach a node in the scene graph
    void attachNode(::Ogre::MovableObject* _node);

    /**
     * @name Slots methods
     * @{
     */
    /// Slot: called when the mesh is modified
    /// Slot: called when the vertices are modified
    void modifyVertices();
    void modifyMesh();
    /// Slot: called when the point colors are modified
    void modifyPointColors();
    /// Slot: called when the texture coordinates are modified
    void modifyTexCoords();
    /// Slot: called when the material is modified
    void modifyMaterial();
    /** @} */

    /// Sets whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera;

    /// Node in the scene graph
    ::Ogre::Entity* m_entity;
    /// Actual mesh data
    ::Ogre::MeshPtr m_ogreMesh;

    /// SMaterial attached to the mesh
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor;
    /// Attached material adaptor UID
    std::string m_materialAdaptorUID;
    /// Ogre Material related to the mesh
    ::fwData::Material::sptr m_material;
    /// Attached Material's name
    std::string m_materialTemplateName;
    /// Texture used to store per-primitive color
    ::Ogre::TexturePtr m_perPrimitiveColorTexture;
    /// Name of the texture used to store per-primitive color
    std::string m_perPrimitiveColorTextureName;

    /// Number of primitives types that are handled by ::fwData::Mesh
    static const unsigned int s_numPrimitiveTypes = ::fwData::Mesh::TETRA + 1;

    /// Pointers on submeshes needed for reallocation check.
    /// For QUADS and TETRAS primitives, they point to r2vb submeshes.
    ::Ogre::SubMesh* m_subMeshes[s_numPrimitiveTypes];
    /// SMaterial adaptors attached to the r2vb objects
    ::visuOgreAdaptor::SMaterial::sptr m_r2vbMaterialAdaptor[s_numPrimitiveTypes];

    /// Maximum size of a texture (TODO: get this from hardware instead)
    static const unsigned int s_maxTextureSize = 2048;

    /// Name of the mesh in Ogre
    std::string m_meshName;
    /// Attached texture adaptor UID
    std::string m_texAdaptorUID;

    /// Binding for each layer
    unsigned short m_binding[NUM_BINDINGS];

    /// Defines if there is a normal layer
    bool m_hasNormal;
    /// Defines if there is a vertex color layer
    bool m_hasVertexColor;
    /// Defines if there is a primitive color layer
    bool m_hasPrimitiveColor;
    /// defines if the mesh changes dynamically, defined in m_configuration
    bool m_isDynamic;
    /// defines if the vertices change dynamically, defined in m_configuration
    bool m_isDynamicVertices;
    /// defines if the mesh has UV coordinates, defined in m_configuration
    bool m_hasUV;
    /// Indicates if the mesh adaptor is managed by a reconstruction adaptor
    bool m_isReconstructionManaged;
    /// Indicates if the mesh adaptor has to create a new material adaptor or simply use the one that is XML configured
    bool m_useNewMaterialAdaptor;

    /// Node containing inputs for the r2vb objects - it will never be inserted in the scene
    ::Ogre::Entity* m_r2vbEntity;
    /// Mesh data for r2vb input - contains only line lists with adjency information primitives
    ::Ogre::MeshPtr m_r2vbMesh;
    /// Name of the r2vb mesh
    std::string m_r2vbMeshName;
    /// List of r2vb objects - these objects triggers the r2vb process and render the output data
    std::vector< ::fwRenderOgre::R2VBRenderable*> m_r2vbObject;
};

//------------------------------------------------------------------------------
// Inline functions

inline SPTR(::fwData::Material) SMesh::getMaterial() const
{
    return m_material;
}

//------------------------------------------------------------------------------

inline void SMesh::setMaterial(::fwData::Material::sptr material)
{
    m_material = material;
}

//------------------------------------------------------------------------------

inline void SMesh::setMaterialTemplateName(const std::string& materialName)
{
    m_materialTemplateName = materialName;
}

//------------------------------------------------------------------------------

inline void SMesh::setAutoResetCamera(bool autoResetCamera)
{
    m_autoResetCamera = autoResetCamera;
}

//------------------------------------------------------------------------------

inline ::Ogre::Entity* SMesh::getEntity() const
{
    return m_entity;
}

//------------------------------------------------------------------------------

inline void SMesh::updateVisibility(bool isVisible)
{
    if(m_entity)
    {
        m_entity->setVisible(isVisible);

        if(m_r2vbEntity)
        {
            m_r2vbEntity->setVisible(isVisible);
        }
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

inline bool SMesh::getVisibility() const
{
    return m_entity ? m_entity->getVisible() : false;
}

//------------------------------------------------------------------------------

inline void SMesh::setDynamic(bool _isDynamic)
{
    m_isDynamic = _isDynamic;
}

//------------------------------------------------------------------------------

inline void SMesh::setDynamicVertices(bool _isDynamic)
{
    m_isDynamicVertices = _isDynamic;
}

//-----------------------------------------------------------------------------

inline void SMesh::setIsReconstructionManaged(bool _isReconstructionManaged)
{
    m_isReconstructionManaged = _isReconstructionManaged;
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SMESH_HPP__

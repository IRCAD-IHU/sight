/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "fwRenderOgre/config.hpp"

#include "fwTools/fwID.hpp"

#include <fwRenderOgre/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreNumerics.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTexture.h>

#include <string>

namespace Ogre
{
class SceneNode;
}

namespace fwRenderOgre
{

/**
 * @brief Manages a plane mesh on which a slice texture will be applied
 */
class FWRENDEROGRE_CLASS_API Plane
{
public:

    typedef ::fwDataTools::helper::MedicalImage::Orientation OrientationMode;

    typedef enum FilteringEnum
    {
        NONE,
        LINEAR,
        ANISOTROPIC
    } FilteringEnumType;

    using sptr = std::shared_ptr<Plane>;

    /// Creates a plane, instantiates its material. Call @ref Plane::initializePlane() to create its geometry.
    FWRENDEROGRE_API Plane(const ::fwTools::fwID::IDType& _negatoId, ::Ogre::SceneNode* _parentSceneNode,
                           ::Ogre::SceneManager* _sceneManager, OrientationMode _orientation, ::Ogre::TexturePtr _tex,
                           FilteringEnumType _filtering, float _entityOpacity = 1.0f);

    /// @deprecated removed in sight 21.0
    [[deprecated("Deprecated constructor. Removed in sight 21.0")]]
    FWRENDEROGRE_API Plane(const ::fwTools::fwID::IDType& _negatoId, ::Ogre::SceneNode* _parentSceneNode,
                           ::Ogre::SceneManager* _sceneManager, OrientationMode _orientation, bool,
                           ::Ogre::TexturePtr _tex, FilteringEnumType _filtering, float _alpha = 1.0f) :
        Plane(_negatoId, _parentSceneNode, _sceneManager, _orientation, _tex, _filtering, _alpha)
    {
    }

    /// Destructor, cleans ogre resources.
    FWRENDEROGRE_API virtual ~Plane();

    /// Instantiates the plane mesh and entity.
    FWRENDEROGRE_API void initializePlane();

    /// Slot handling slice plane move:
    ///     - in 2D, it will convert the position in unit floating value and call the fragment shader
    ///     - in 3D, it will also move the scene node in space
    FWRENDEROGRE_API void changeSlice( float _sliceIndex );

    /// Sets the image axis orthogonal to the plane.
    FWRENDEROGRE_API void setOrientationMode(OrientationMode _newMode);

    /// Sets whether the negato's opacity is taken into account.
    FWRENDEROGRE_API void enableAlpha(bool);

    /// Sets the real world image's origin .
    FWRENDEROGRE_API void setOriginPosition(const ::Ogre::Vector3& _origPos);

    /// Sets the real world size of a voxel.
    FWRENDEROGRE_API void setVoxelSpacing(const ::Ogre::Vector3& _spacing);

    /// Sets the plane's opacity.
    FWRENDEROGRE_API void setEntityOpacity( float _f );

    /// Shows/hides the plane in the scene.
    FWRENDEROGRE_API void setVisible( bool _visible );

    /// Adds or updates the texture containing the transfer function data in the negato passes
    FWRENDEROGRE_API void setTFData(const ::fwRenderOgre::TransferFunction& _tfTexture);

    /// Sets whether or not the transfer function uses thresholding.
    FWRENDEROGRE_API void switchThresholding(bool _threshold);

    /// Retrieves the plane's width in model space.
    FWRENDEROGRE_API ::Ogre::Real getWidth() const;

    /// Retrieves the plane's height in model space.
    FWRENDEROGRE_API ::Ogre::Real getHeight() const;

    /// Moves the scene node to m_originPosition point
    FWRENDEROGRE_API void moveToOriginPosition();

    /// Returns the x, y or z world position of the plane scene node according to the current orientation mode
    FWRENDEROGRE_API double getSliceWorldPosition() const;

    /// Retrieves the image axis orthogonal to the plane.
    FWRENDEROGRE_API OrientationMode getOrientationMode() const;

    /// Retrieves the material used to render the plane.
    FWRENDEROGRE_API ::Ogre::MaterialPtr getMaterial();

    /// Retrieves the movable object created by this class.
    FWRENDEROGRE_API const ::Ogre::MovableObject* getMovableObject() const;

    /// Sets the picking flags.
    FWRENDEROGRE_API void setQueryFlags(std::uint32_t _flags);

    /// Sets this object's render queue group and render priority.
    FWRENDEROGRE_API void setRenderQueuerGroupAndPriority(std::uint8_t _groupId, std::uint16_t _priority);

private:

    /// Sets the plane's original position.
    void initializePosition();

    /// Creates a material for the mesh plane with the negato texture
    void initializeMaterial();

    /// Sets the relativePosition, then
    void setRelativePosition(float _relativePosition);

    /// Moves plane along its Normal.
    void moveAlongAxis();

    /// Sets the dimensions for the related members, and also creates a movable plane to instanciate the entity.
    ::Ogre::MovablePlane setDimensions();

    /// Indicates whether whe want to threshold instead of windowing
    bool m_threshold { false };

    /// Defines the filtering type for this plane
    FilteringEnumType m_filtering { FilteringEnum::ANISOTROPIC };

    /// Orientation mode of the plane
    OrientationMode m_orientation;

    /// The plane on wich we will apply a texture
    ::Ogre::MeshPtr m_slicePlane;
    /// The origin position of the slice plane according to the source image's origin
    ::Ogre::Vector3 m_originPosition { ::Ogre::Vector3::ZERO };
    /// Pointer to the material
    ::Ogre::MaterialPtr m_texMaterial;
    /// Pointer to the texture
    ::Ogre::TexturePtr m_texture;
    /// Points to the scenemanager containing the plane
    ::Ogre::SceneManager* m_sceneManager { nullptr };

    /// Strings needed to initialize mesh, scenenode, etc.
    std::string m_slicePlaneName;
    /// Entity name. used to recover this from the Ogre entityManager
    std::string m_entityName;
    /// Used to recover the scenenode from it's name.
    std::string m_sceneNodeName;

    /// The scene node on which we will attach the mesh
    ::Ogre::SceneNode* m_planeSceneNode { nullptr };
    /// The parent scene node.
    ::Ogre::SceneNode* m_parentSceneNode { nullptr };

    /// Entity's width.
    ::Ogre::Real m_width { 0.f };
    /// Entity's height.
    ::Ogre::Real m_height { 0.f };
    /// Entity's depth.
    ::Ogre::Real m_depth { 0.f };

    /// Spacing in the texture 3d image file.
    ::Ogre::Vector3 m_spacing { ::Ogre::Vector3::ZERO };

    /// Where are we insite the depth range?
    float m_relativePosition { 0.8f };

    /// Opacity applied to the entity.
    float m_entityOpacity { 1.f };
};

//------------------------------------------------------------------------------
// Inline functions

inline void Plane::setOriginPosition(const ::Ogre::Vector3& _origPos)
{
    m_originPosition = _origPos;
}

//------------------------------------------------------------------------------

inline Plane::OrientationMode Plane::getOrientationMode() const
{
    return m_orientation;
}

//------------------------------------------------------------------------------

inline ::Ogre::MaterialPtr Plane::getMaterial()
{
    return m_texMaterial;
}

//------------------------------------------------------------------------------

inline ::Ogre::Real Plane::getWidth() const
{
    return m_width;
}

//------------------------------------------------------------------------------

inline ::Ogre::Real Plane::getHeight() const
{
    return m_height;
}

//------------------------------------------------------------------------------

} // Namespace fwRenderOgre

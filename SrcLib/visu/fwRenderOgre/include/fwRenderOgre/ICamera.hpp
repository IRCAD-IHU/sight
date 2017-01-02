/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_ICAMERA_HPP__
#define __FWRENDEROGRE_ICAMERA_HPP__

#include "fwRenderOgre/IAdaptor.hpp"
#include "fwRenderOgre/SRender.hpp"
#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/factory/new.hpp"
#include "fwRenderOgre/registry/detail.hpp"

#include <fwGui/container/fwContainer.hpp>

#include <OGRE/OgreCamera.h>

#include <string>

//namespace Ogre
//{
//class Camera;
//}

namespace fwRenderOgre
{

/**
 * @brief
 *
 */
class FWRENDEROGRE_CLASS_API ICamera : public ::fwRenderOgre::IAdaptor
{

public:

    typedef ::fwRenderOgre::cameraFactory::Key Key;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar(std::string functorKey)
        {
            ::fwRenderOgre::registry::getCameraRegistry()->addFactory(functorKey,
                                                                      &::fwRenderOgre::cameraFactory::New<T>);
        }
    };

    fwCoreNonInstanciableClassDefinitionsMacro( (ICamera)(::fwRenderOgre::IAdaptor) )

    typedef std::string FactoryRegistryKeyType;

    FWRENDEROGRE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    FWRENDEROGRE_API static ICamera::sptr createCameraManager();

    /// Constructor. Do nothing.
    FWRENDEROGRE_API ICamera();

    /// Destructor. Do nothing.
    FWRENDEROGRE_API virtual ~ICamera();

    FWRENDEROGRE_API virtual ::Ogre::Camera* getCamera()                                                      = 0;
    FWRENDEROGRE_API virtual void createCamera(const std::string& _name, ::Ogre::SceneManager* _sceneManager) = 0;
    FWRENDEROGRE_API virtual void setNearClipDistance(::Ogre::Real _nearClipDistance)                         = 0;
    FWRENDEROGRE_API virtual void setFarClipDistance(::Ogre::Real _farClipDistance)                           = 0;
    FWRENDEROGRE_API virtual void setAspectRatio(::Ogre::Real _ratio)                                         = 0;
    FWRENDEROGRE_API virtual void setTransformName(::fwRenderOgre::SRender::OgreObjectIdType newId)           = 0;
};

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_ICAMERA_HPP__

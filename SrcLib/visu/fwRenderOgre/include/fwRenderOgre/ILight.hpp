/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_ILIGHT_HPP__
#define __FWRENDEROGRE_ILIGHT_HPP__

#include "fwRenderOgre/IAdaptor.hpp"
#include "fwRenderOgre/SRender.hpp"
#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/factory/new.hpp"
#include "fwRenderOgre/registry/detail.hpp"

#include <fwData/Color.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwGui/container/fwContainer.hpp>

#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreLight.h>
#include <OGRE/OgreVector3.h>

#include <string>

namespace fwRenderOgre
{

/**
 * @brief
 *
 */
class FWRENDEROGRE_CLASS_API ILight : public ::fwRenderOgre::IAdaptor
{

public:

    typedef ::fwRenderOgre::lightFactory::Key Key;

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
            ::fwRenderOgre::registry::getLightRegistry()->addFactory(functorKey, &::fwRenderOgre::lightFactory::New<T>);
        }
    };

    fwCoreNonInstanciableClassDefinitionsMacro( (ILight)(::fwRenderOgre::IAdaptor) )

    typedef std::string FactoryRegistryKeyType;

    FWRENDEROGRE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    FWRENDEROGRE_API static ILight::sptr createLightManager(::fwData::TransformationMatrix3D::sptr _transform,
                                                            ::fwData::Color::sptr _diffuse,
                                                            ::fwData::Color::sptr _specular);

    /// Constructor. Do nothing.
    FWRENDEROGRE_API ILight();

    /// Destructor. Do nothing.
    FWRENDEROGRE_API virtual ~ILight();

    FWRENDEROGRE_API virtual void setName(std::string _name)
        = 0;
    FWRENDEROGRE_API virtual void setType(::Ogre::Light::LightTypes _type)
        = 0;
    FWRENDEROGRE_API virtual void setDiffuseColor(::Ogre::ColourValue _diffuseColor)
        = 0;
    FWRENDEROGRE_API virtual void setSpecularColor(::Ogre::ColourValue _specularColor)
        = 0;
    FWRENDEROGRE_API virtual void setParentTransformName(
        const ::fwRenderOgre::SRender::OgreObjectIdType& _parentTransformName) = 0;
};

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_ILIGHT_HPP__



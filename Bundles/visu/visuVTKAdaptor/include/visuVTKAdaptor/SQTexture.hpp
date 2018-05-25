/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SQTexture_HPP__
#define __VISUVTKADAPTOR_SQTexture_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IQAdaptor.hpp>

namespace fwData
{
class Material;
}

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor to map a texture on a mesh.
 *
 * This is done via ::visuVTKAdaptor::SMaterial. However in the configuration we don't specify the material adaptor
 * since it is automatically created by the ::visuVTKAdaptor::SMesh adaptor. Thus we rather try to specify the mesh
 * adaptor.
 *
 * To use this adaptor, you need to connect it to the SMesh::textureApplied signal
 *
 * @code{.qml}
    <connect>
        <signal>meshAdaptor/textureApplied</signal>
        <slot>textureAdaptor/applyTexture</slot>
    </connect>
   @endcode
 *
 * @section Slots Slots
 * - \b applyTexture(SPTR(::fwData::Material)): update image slice type
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="::visuVTKAdaptor::SQTexture" autoConnect="yes">
       <inout key="texture" uid="..." />
       <config texture="texture" filtering="linear" wrapping="repeat" />
   </service>
   @endcode
 *
 *  * @subsection In-Out In-Out
 * - \b texture [::fwData::Image]: image to use as texture.
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b filtering (optional) : filtering of the texture, "nearest" or "linear"
 *    - \b wrapping (optional) : wrapping of the texture, "clamp" or "repeat"
 *    - \b lighting (optional) : enable the lighting, "yes" or "no" default yes
 */
class VISUVTKADAPTOR_CLASS_API SQTexture : public ::fwRenderVTK::IQAdaptor
{

    Q_OBJECT

    Q_PROPERTY(fwServices::QtObjectHolder *texture MEMBER m_texture)

public:

    /**
     * @name Slots API
     * @{
     */
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_APPLY_TEXTURE_SLOT;
    typedef ::fwCom::Slot< void ( SPTR(::fwData::Material) ) > ApplyTextureSlotType;
    /** @} */

    /// Constructor
    VISUVTKADAPTOR_API SQTexture() noexcept;
    /// Destructor
    VISUVTKADAPTOR_API virtual ~SQTexture() noexcept;

public Q_SLOTS:
    void    applyTexture(QObject *);

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;


    /// Slot called when a texture must be applied on a material.
    void applyTexture( SPTR(::fwData::Material) _material);

    /// Contains all mesh adaptors that currently have this texture applied. Needed when image is updated.
    std::set< SPTR(::fwData::Material) > m_materialSet;

    /// How to filter this texture
    std::string m_filtering;

    /// How to wrap the texture
    std::string m_wrapping;

    /// enable or not the lighting (default true)
    bool m_lighting;

    ::fwServices::QtObjectHolder    *m_texture = nullptr;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SQTexture_HPP__
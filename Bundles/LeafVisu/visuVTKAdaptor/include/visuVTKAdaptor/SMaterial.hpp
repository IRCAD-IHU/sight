/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SMATERIAL_HPP__
#define __VISUVTKADAPTOR_SMATERIAL_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

class VISUVTKADAPTOR_CLASS_API vtkProperty;

namespace fwData
{
class Material;
}

namespace visuVTKAdaptor
{

/**
 * @brief Manage material representation of meshes
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SMaterial">
       <in key="material" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b input [::fwData::Material]: material applied on the mesh.
 */
class VISUVTKADAPTOR_CLASS_API SMaterial : public ::fwRenderVTK::IAdaptor
{

public:

    static const ::fwServices::IService::KeyType s_MATERIAL_INPUT;

    fwCoreServiceClassDefinitionsMacro( (SMaterial)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SMaterial() noexcept;

    VISUVTKADAPTOR_API virtual ~SMaterial() noexcept;

    VISUVTKADAPTOR_API void setVtkProperty(vtkProperty* property);

    VISUVTKADAPTOR_API vtkProperty* getVtkProperty() const;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Material::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    void updateMaterial( CSPTR(::fwData::Material) material );

    vtkProperty* m_property;
    bool m_manageProperty;
    bool m_lighting;

};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SMATERIAL_HPP__

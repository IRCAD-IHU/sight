/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __VISUVTKADAPTOR_SRESECTION_HPP__
#define __VISUVTKADAPTOR_SRESECTION_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

namespace visuVTKAdaptor
{

/**
 * @brief This service displays a ResectionDB (it launchs Reconstruction adators)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SResection" autoConnect="yes">
       <in key="resection" uid="..." />
       <config renderer="default" transform="..." picker="" />
   </service>
   @endcode
 * @subsection Input Input
 * - \b resection [::fwData::Resection]: resection to display.
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer(mandatory) : renderer where the resection is displayed
 *    - \b picker(optional) : picker used to pick on the resection
 *    - \b autoresetcamera(optional, default: yes) : reset the camera point of view when the data is modified ("yes" or
 *         "no", default: "yes").
 *    - \b clippingplanes(optional) : identifier of the clipping planes
 */
class VISUVTKADAPTOR_CLASS_API SResection : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SResection)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SResection() noexcept;

    VISUVTKADAPTOR_API virtual ~SResection() noexcept;

    static const ::fwServices::IService::KeyType s_RESECTION_INPUT;

    void setClippingPlanes(::fwRenderVTK::SRender::VtkObjectIdType id);

    /// Active/Inactive automatic reset on camera for triangular mesh adaptor. By default =true.
    void setAutoResetCamera(bool autoResetCamera);

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect mesh::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect mesh::s_VISIBILITY_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect mesh::s_RECONSTRUCTION_ADDED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

private:
    ::fwRenderVTK::SRender::VtkObjectIdType m_clippingPlanes;
    bool m_autoResetCamera;
};

//------------------------------------------------------------------------------

inline void SResection::setClippingPlanes(::fwRenderVTK::SRender::VtkObjectIdType id)
{
    m_clippingPlanes = id;
}

//------------------------------------------------------------------------------

inline void SResection::setAutoResetCamera(bool autoResetCamera)
{
    m_autoResetCamera = autoResetCamera;
}

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SRESECTION_HPP__

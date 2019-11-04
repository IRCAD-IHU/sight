/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwCom/Slot.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/Text.hpp>

#include <Ogre.h>

namespace fwData
{
class Material;
}

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor shows a simple coordinate system.
 *
 * @section Slots Slots
 * -\b updateVisibility(bool): Sets whether the axis is shown or not.
 * -\b toggleVisibility(): Toggle whether the axis is shown or not.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SAxis">
        <config layer="default" transform="transformUID" length="30" label="true" />
    </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b layer (mandatory) : defines the mesh's layer.
 * - \b transform (optional): the name of the Ogre transform node where to attach the mesh, as it was specified
 * in the STransform adaptor.
 * - \b length (optional, float, default=50.f): axis length in scene units.
 * - \b label (optional, bool, default=true): display axis names.
 * - \b fontSize (optional, default=16): label font size in points.
 *
 */
class VISUOGREADAPTOR_CLASS_API SAxis : public ::fwRenderOgre::IAdaptor,
                                        public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceMacro(SAxis, ::fwRenderOgre::IAdaptor)

    /// Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SAxis() noexcept;
    /// Does nothing.
    VISUOGREADAPTOR_API virtual ~SAxis() noexcept;

private:

    /// Returns proposals to connect service slots to associated object signals.
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;
    /// Configures the adaptor.
    void configuring() override;
    /// Manually creates a Mesh in the Default Ogre Ressource group.
    void starting() override;
    /// Checks if the fwData::Mesh has changed, and updates it if it has.
    void updating() override;
    /// Deletes the mesh after unregistering the service, and shutting connections.
    void stopping() override;

    /// Sets visibility of axis.
    void updateVisibility(bool isVisible);
    /// Toggle visibility of axis.
    void toggleVisibility();

    /// Pointer to the Material data.
    ::fwData::Material::sptr m_material { nullptr };
    /// Axis length (in mm).
    float m_length { 50.f };
    /// Handles axis visibility.
    bool m_isVisible { true };
    /// Handles the visibility of axis labels.
    bool m_enableLabel { true };

    /// Line along the x axis.
    ::Ogre::ManualObject* m_xLine { nullptr };
    /// Line along the y axis.
    ::Ogre::ManualObject* m_yLine { nullptr };
    /// Line along the z axis.
    ::Ogre::ManualObject* m_zLine { nullptr };

    /// Arrow of the x axis.
    ::Ogre::ManualObject* m_xCone { nullptr };
    /// Arrow of the y axis.
    ::Ogre::ManualObject* m_yCone { nullptr };
    /// Arrow of the z axis.
    ::Ogre::ManualObject* m_zCone { nullptr };

    /// Scene node where all of our manual objects are attached.
    ::Ogre::SceneNode* m_sceneNode { nullptr };

    /// Labels attached to each axis.
    std::array< ::fwRenderOgre::Text*, 3> m_axisLabels {{ nullptr, nullptr, nullptr }};

    /// Label font size in points.
    size_t m_fontSize { 16 };

};

} //namespace visuOgreAdaptor

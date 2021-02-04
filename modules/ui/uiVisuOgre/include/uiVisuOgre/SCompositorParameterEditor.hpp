/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "uiVisuOgre/config.hpp"

#include <QVBoxLayout>

#include <ui/base/editor/IEditor.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <viz/ogre/Layer.hpp>
#include <viz/ogre/SRender.hpp>

namespace uiVisuOgre
{

/**
 * @brief   Editor allowing to edit shader uniform from a compositor material.

 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="::uiVisuOgre::SCompositorParameterEditor" >

       </service>
       @endcode
 */
class UIVISUOGRE_CLASS_API SCompositorParameterEditor : public sight::ui::base::editor::IEditor
{

public:

    fwCoreServiceMacro(SCompositorParameterEditor, sight::ui::base::editor::IEditor)

    /// Constructor.
    UIVISUOGRE_API SCompositorParameterEditor() noexcept;

    /// Destructor. Destroy UI
    UIVISUOGRE_API virtual ~SCompositorParameterEditor() noexcept;

    /**
     * @name Slots API
     * @{
     */
    /// Slot: update the interface.
    UIVISUOGRE_API static const core::com::Slots::SlotKeyType s_UPDATE_COMPOSITOR_SLOT;
    /** @} */

protected:

    /// Configure the editor to associate with each object type
    UIVISUOGRE_API virtual void configuring() override;

    /// Start the service, slot connections, QtContainer initialization .
    UIVISUOGRE_API virtual void starting() override;

    /// Destroy the service and the container.
    UIVISUOGRE_API virtual void stopping() override;

    /// Update the interface.
    UIVISUOGRE_API virtual void updating() override;

private:
    /// Slot: update the interface.
    void updateCompositor(std::string _compositorName, bool _enabled, viz::ogre::Layer::sptr _layer);

    /// Clear the current container
    void clear();
    /// Instanciates the needed ui editors according to the stored informations
    void fillGui();

    /// Internal class that contain the informations concerning the editor that is created.
    struct ShaderEditorInfo
    {
        std::string uuid;
        sight::ui::qt::container::QtContainer::sptr editorPanel;
        services::IService::wptr service;
        core::com::helper::SigSlotConnection connections;
    };

    ShaderEditorInfo m_editorInfo;

    QVBoxLayout* m_sizer;

    /// Name of the layer when we configure it in XML
    std::string m_layerID;
};

} // uiVisuOgre

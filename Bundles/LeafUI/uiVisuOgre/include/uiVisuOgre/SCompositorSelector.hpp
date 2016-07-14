/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUOGRE_SCOMPOSITORSELECTOR_HPP__
#define __UIVISUOGRE_SCOMPOSITORSELECTOR_HPP__

#include <gui/editor/IEditor.hpp>
#include <fwRenderOgre/Layer.hpp>
#include <fwRenderOgre/compositor/ChainManager.hpp>

#include <QObject>
#include <QPointer>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QListWidget>

#include <vector>

#include "uiVisuOgre/config.hpp"

namespace uiVisuOgre
{

/**
 * @brief   Allows to select an Ogre Compositor and apply it to a layer
 */
class UIVISUOGRE_CLASS_API SCompositorSelector : public QObject,
                                                 public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (SCompositorSelector)(::gui::editor::IEditor) );


    /** @} */

    /**
     * @name Slots API
     * @{
     */
    typedef ::fwCom::Slot< void (::fwRenderOgre::Layer::sptr) > InitLayerSlotType;

    /// Slot: Populate the list of available compositors for the selected layer
    UIVISUOGRE_API static const ::fwCom::Slots::SlotKeyType s_INIT_COMPOSITOR_LIST_SLOT;

    /** @} */

    /// Constructor.
    UIVISUOGRE_API SCompositorSelector() throw();

    /// Destructor. Does nothing
    UIVISUOGRE_API virtual ~SCompositorSelector() throw();

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="SCompositorSelectorInstance" impl="::uiVisuOgre::SCompositorSelector" type="::gui::editor::IEditor">
             <parameter>param</parameter>
        </service>
       @endcode
     * - \b Parameter : parameter description.
     */
    UIVISUOGRE_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Sets the connections and the UI elements
    UIVISUOGRE_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Destroys the connections and cleans the container
    UIVISUOGRE_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Does nothing
    UIVISUOGRE_API virtual void updating() throw ( ::fwTools::Failed );

private Q_SLOTS:

    /// Slot: called when a layer is selected
    /// Sets the current layer and initializes the compositor list
    void onSelectedLayerItem(int index);

    /// Slot: called when an item of the list widget is checked
    void onSelectedCompositorItem(QListWidgetItem* compositorItem);

private:
    /// Slot: Populate the list of available compositors for the selected layer
    void initCompositorList(::fwRenderOgre::Layer::sptr layer);

    /// Retrieves all the layers from the application thanks to the render services
    void refreshRenderers();

    /// Checks if there is an XML configured compositor chain.
    /// Sets the local compositor chain according to this.
    void synchroniseWithLayerCompositorChain();

    /// Retrieves the available compositors from the compositor resource group and stores them in the list widget
    void updateCompositorList();

    /// Iterates through the compositor chain and checks the enabled compositors
    void checkEnabledCompositors();

    /// Iterates through the compositor chain and unckecks them
    void uncheckCompositors();

    /// Indicates if a compositor is enabled on the layer
    bool isEnabledCompositor(const std::string& compositorName);

    QPointer<QComboBox> m_layersBox;

    QPointer<QListWidget> m_compositorChain;

    std::vector< ::fwRenderOgre::Layer::wptr > m_layers;
    ::fwRenderOgre::Layer::wptr m_currentLayer;

    ::fwRenderOgre::compositor::ChainManager::CompositorChainType m_layerCompositorChain;

    ///Connection service, needed for slot/signal association
    ::fwServices::helper::SigSlotConnection m_connections;
};

} // uiVisuOgre

#endif // __UIVISUOGRE_SCOMPOSITORSELECTOR_HPP__

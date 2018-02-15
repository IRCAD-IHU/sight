/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "guiQt/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <QListWidget>
#include <QPointer>

namespace guiQt
{

namespace editor
{

/**
 * @brief   This editor allows to draw a list.
 * The currently selected item can be deleted by pressing on "del" key
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::guiQt::editor::SListView"/>
    </service>
   @endcode
 *
 * @section Signals Signals
 * - \b itemAdded(int): This editor emits the signal "itemAdded" with the index value of added item.
 * - \b itemRemoved(int): This editor emits the signal "itemRemoved" with the index value of removed item.
 * - \b itemDoubleClicked(int): This editor emits the signal "itemDoubleClicked" with the index value of selected item.
 *
 * @section Slots Slots
 * - \b insertItem(int, std::string): This slot allows to add an item with its index and text.
 * - \b removeItem(int): This slot allows to remove the element at the index.
 */
class GUIQT_CLASS_API SListView : public QObject,
                                  public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SListView)(::fwGui::editor::IEditor) );

    /// Constructor. Do nothing.
    GUIQT_API SListView() noexcept;

    /// Destructor. Do nothing.
    GUIQT_API virtual ~SListView() noexcept;

    /**@name Signals API
     * @{
     */

    GUIQT_API static const ::fwCom::Signals::SignalKeyType s_ITEM_ADDED_SIG;
    typedef ::fwCom::Signal<void (int)> ItemAddedSignalType;

    GUIQT_API static const ::fwCom::Signals::SignalKeyType s_ITEM_REMOVED_SIG;
    typedef ::fwCom::Signal<void (int)> ItemRemovedSignalType;

    GUIQT_API static const ::fwCom::Signals::SignalKeyType s_ITEM_DOUBLE_CLICKED_SIG;
    typedef ::fwCom::Signal<void (int)> ItemDoubleClickedSignalType;

    /** @} */

    /**
     * @name Slots API
     * @{
     */

    GUIQT_API static const ::fwCom::Slots::SlotKeyType s_INSERT_ITEM_SLOT;
    GUIQT_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_ITEM_SLOT;

    /// SLOT : Called to insert an item at index.
    void insertItem( int index, std::string value );

    /// SLOT : Called to remove the item at the index position.
    void removeItem(int index);

    ///@}

protected:

    /// used to catch the del key released event
    bool eventFilter(QObject* watched, QEvent* event) override;

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Installs the layout
    virtual void starting() override;

    /// Destroys the layout
    virtual void stopping() override;

    /// Does nothing
    virtual void updating() override;

    /// Does nothing
    virtual void swapping() override;

    /// Configure the service
    virtual void configuring() override;

private:

    QPointer<QListWidget> m_listWidget; /// listWidget

private Q_SLOTS:

    /// called when an item is double clicked
    void onItemDoubleClicked(QListWidgetItem* item);
};

}//namespace editor
}//namespace guiQt

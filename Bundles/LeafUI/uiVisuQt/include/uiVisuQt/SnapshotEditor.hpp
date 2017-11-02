/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUQT_SNAPSHOTEDITOR_HPP__
#define __UIVISUQT_SNAPSHOTEDITOR_HPP__

#include "uiVisuQt/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QPushButton;

namespace uiVisuQt
{

/**
 * @brief   SnapshotEditor service is represented by a button. It allows to snap shot a generic scene.
 *
 * Send a 'snapped' signal containing the filename used to save the snapshot.
 * @note You need to connect the 'snapped' signal to one visuVTKAdaptor::Snapshot to save the file.
 */
class UIVISUQT_CLASS_API SnapshotEditor : public QObject,
                                          public ::gui::editor::IEditor
{

Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SnapshotEditor)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIVISUQT_API SnapshotEditor() noexcept;

    /// Destructor. Do nothing.
    UIVISUQT_API virtual ~SnapshotEditor() noexcept;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Install the layout.
     */
    virtual void starting() override;

    /**
     * @brief Destroy the layout.
     */
    virtual void stopping() override;

    /// Do nothing
    virtual void updating() override;

    /// Do nothing
    virtual void swapping() override;

    /**
     * @brief Configure the editor.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="snapshotEditor" type="::gui::editor::IEditor" impl="::uiVisuQt::SnapshotEditor" autoConnect="no" />
       @endcode
     */
    virtual void configuring() override;

    /// Overrides
    virtual void info( std::ostream& _sstream ) override;

protected Q_SLOTS:
    /**
     * @brief Show a file dialog and notify the scene must be printed.
     */
    void onSnapButton();

private:
    std::string requestFileName();

    /**
     * @name Signals
     * @{
     */
    /// Type of signal to snap shot
    typedef ::fwCom::Signal< void (std::string) > SnappedSignalType;
    static const ::fwCom::Signals::SignalKeyType s_SNAPPED_SIG;

    SnappedSignalType::sptr m_sigSnapped; ///< snap shot signal
    /**
     * @}
     */

    QPointer< QPushButton > m_snapButton;

};

} // uiVisuQt

#endif /*__UIVISUQT_SNAPSHOTEDITOR_HPP__*/


/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "uiVisu/config.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>
#include <core/tools/Failed.hpp>

#include <gui/IActionSrv.hpp>

#include <map>
#include <string>

namespace uiVisu
{
namespace action
{

/**
 * @brief   This action allows to snapshot a generic scene
 *
 * Send a 'snapped' signal containing the filename used to save the snapshot.
 * @note You need to connect the 'snapped' signal to one visuVTKAdaptor::Snapshot to save the file.
 */
class UIVISU_CLASS_API SSnapshot : public ::sight::gui::IActionSrv
{

public:

    fwCoreServiceMacro(SSnapshot, ::sight::gui::IActionSrv)

    /// Constructor. Do nothing.
    UIVISU_API SSnapshot() noexcept;

    /// Destructor. Do nothing.
    UIVISU_API virtual ~SSnapshot() noexcept;

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    /// Starts action
    virtual void starting() override;

    /// Stops action
    virtual void stopping() override;

    /// Show a file dialog and notify the scene which must be printed.
    void updating() override;

    /// Does nothing
    void swapping() override;

    /**
     * @brief Configure action.
     * @code{.xml}
       <service type="::gui::IActionSrv" impl="::uiData::action::SSnapshot" autoConnect="no" />
       @endcode
     */
    void configuring() override;

    virtual void info( std::ostream& _sstream ) override;

private:

    /**
     * @name Signals
     * @{
     */
    /// Type of signal to snap shot
    typedef core::com::Signal< void (std::string) > SnappedSignalType;
    static const core::com::Signals::SignalKeyType s_SNAPPED_SIG;

    SnappedSignalType::sptr m_sigSnapped; ///< snap shot signal
    /**
     * @}
     */

    std::string requestFileName();

};

} // action
} // uiVisu

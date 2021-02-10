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

#include "modules/memory/config.hpp"

#include <data/ObjectLock.hpp>

#include <services/IController.hpp>

namespace sight::modules::memory
{

/**
 * @brief  This services dump lock object. Unlock it on stopping.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::sight::modules::memory::LockDumpSrv">
       <inout key="target" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b target [sight::data::Object]: object to dump lock.
 */
class MODULE_MEMORY_CLASS_API LockDumpSrv : public services::IController
{

public:

    fwCoreServiceMacro(LockDumpSrv, sight::services::IController)

    /// Constructor. Does nothing
    MODULE_MEMORY_API LockDumpSrv() noexcept;

    /// Destructor. Does nothing
    MODULE_MEMORY_API virtual ~LockDumpSrv() noexcept;

protected:

    /// Uses data::ObjectLock to dump lock the associated data
    MODULE_MEMORY_API virtual void starting() override;

    /// Dump unlock the associated data
    MODULE_MEMORY_API virtual void stopping() override;

    /// Does nothing
    MODULE_MEMORY_API virtual void reconfiguring() override;

    /// Does nothing
    MODULE_MEMORY_API virtual void updating() override;

    /// Does nothing
    MODULE_MEMORY_API virtual void info( std::ostream& _sstream ) override;

    /// Does nothing
    MODULE_MEMORY_API virtual void configuring() override;

private:

    data::ObjectLock m_objLock;

};

} // sight::modules::memory

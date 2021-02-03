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

#include "memory/config.hpp"

#include <core/memory/IPolicy.hpp>
#include <core/tools/Failed.hpp>

#include <fwServices/IController.hpp>

namespace memory
{

class MEMORY_CLASS_API SDumpPolicy : public ::fwServices::IController
{

public:

    fwCoreServiceMacro(SDumpPolicy, ::fwServices::IController)

    MEMORY_API SDumpPolicy();

    MEMORY_API virtual ~SDumpPolicy();

    /**
     * Configuration example :
       @code{.xml}
       <service ...>
       <config>
         <policy>ValveDump</policy>
         <params>
          <min_free_mem>524288000</min_free_mem>
          <hysteresis_offet>104857600</hysteresis_offet>
         </params>
       </config>
       </service>
       @endcode
     * */
    MEMORY_API virtual void configuring() override;

    /// Overrides
    MEMORY_API virtual void starting() override;

    /// Overrides
    MEMORY_API virtual void stopping() override;

    /// Overrides
    MEMORY_API virtual void updating() override;

    /// Overrides
    MEMORY_API virtual void swapping() override;

protected:

    typedef std::vector< std::pair< core::memory::IPolicy::ParamNamesType::value_type, std::string > > ParametersType;

    std::string m_policy;
    ParametersType m_policyParams;
};

} // namespace memory

/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "modules/geometry/base/config.hpp"

#include <core/com/Signal.hpp>

#include <data/Matrix4.hpp>

#include <service/IOperator.hpp>

namespace sight::module::geometry::base
{

/**
 * @brief Extracts translation part of multiple matrices and put it into a pointlist.
 * The pointlist is cleared by default. It can be set to 'appending' mode through the configuration.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="sight::module::geometry::base::SPointListFromMatrices">
            <in group="matrices">
                <key uid="matrix1" />
                <key uid="matrix2" />
                <key uid="matrix3" />
            </in>
            <inout key="pointList"/>
            <config append="false"/>
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b matrices [sight::data::Matrix4]: input matrices (only translation part will be extracted).
 *
 * @subsection Inout Inout
 * - \b pointList [sight::data::PointList]: output pointlist, each point represent translation part of corresponding
 * matrix
 *
 * @subsection Configuration Configuration
 * - \b append (optional, default=false): does the service append points in the pointlist on update (true) or clear it
 * (false)?
 */
class MODULE_GEOMETRY_BASE_CLASS_API SPointListFromMatrices final : public sight::service::IOperator
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SPointListFromMatrices, sight::service::IOperator);

    /// Initializes a member variable
    MODULE_GEOMETRY_BASE_API SPointListFromMatrices();

    /// Destroys the service
    MODULE_GEOMETRY_BASE_API ~SPointListFromMatrices() override;

protected:

    /// Sets the append mode on or off.
    MODULE_GEOMETRY_BASE_API void configuring() override;

    /// Does nothing.
    MODULE_GEOMETRY_BASE_API void starting() override;

    /// Fills the PointList.
    MODULE_GEOMETRY_BASE_API void updating() override;

    /// Does nothing.
    MODULE_GEOMETRY_BASE_API void stopping() override;

private:

    /// Append mode
    bool m_append;
};

} // namespace sight::module::geometry::base.

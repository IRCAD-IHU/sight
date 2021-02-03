/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "validators/config.hpp"

#include <activities/IObjectValidator.hpp>

namespace validators
{
namespace ModelSeries
{
/**
 * @brief Defines a validator which checks that the ModelSeries contains one and only one Tool.
 * @note If the given Obect is a container (Vector or Composite), it validates that each sub-object is a ModelSeries
 *       with one and only one Tool.
 */
class VALIDATORS_CLASS_API ContainOneTool : public activities::IObjectValidator
{
public:
    fwCoreClassMacro(ContainOneTool, activities::IObjectValidator,
                     activities::validator::factory::New< ContainOneTool >)

    /// Constructor. Do nothing.
    VALIDATORS_API ContainOneTool(activities::IValidator::Key key);

    /// Destructor. Do nothing.
    VALIDATORS_API virtual ~ContainOneTool();

    /**
     * @brief Checks that the ModelSeries contains one only one Tool.
     * @note Given object should be a single ModelSeries or a Vector or a Composite of ModelSeries.
     * @see activities::IValidator::validate
     */
    VALIDATORS_API virtual IValidator::ValidationType validate(const CSPTR(data::Object)& currentObject ) const
    override;
};

} // namespace ModelSeries
} // namespace validators

/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "modules/filter/image/config.hpp"

#include <services/IRegisterer.hpp>

namespace sight::modules::filter::image
{
/**
 * @brief This service uses a fast registration algorithm to quickly register two images together if the transform
 * between both is pure translation.
 *
 * @section Signals Signals
 *
 * @section Slots Slots
 *   - \b update: compute the registration
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="::sight::modules::filter::image::SFastRegistration">
       <in key="source" uid="..." />
       <in key="target" uid="..." />
       <in key="transform" uid="..." />
       <inout key="flipTransform" uid="..." />
   </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b source [data::Image]: Input image that will be aligned to the target.
 * - \b target [data::Image]: Input image to which the source will be aligned.
 * - \b flipTransform [data::TransformationMatrix3D]: temporary hack allowing to flip the image according to the
 *   flipping applied in the RegistrationActivity.
 *
 * @subsection In-Out In-Out
 * - \b transform [data::TransformationMatrix3D]: initial registration. The computed registration will be
 *   concatenated to the initial registration matrix.
 */
class MODULE_FILTER_IMAGE_CLASS_API SMIPMatchingRegistration : public services::IRegisterer
{
public:
    fwCoreServiceMacro(SMIPMatchingRegistration, services::IRegisterer)

    /**
     * @brief Constructor
     */
    MODULE_FILTER_IMAGE_API SMIPMatchingRegistration() noexcept;
    /**
     * @brief Destructor
     */
    MODULE_FILTER_IMAGE_API virtual ~SMIPMatchingRegistration() noexcept;

protected:
    /**
     * @brief Starts the service.
     *
     * @throw core::tools::Failed
     */
    MODULE_FILTER_IMAGE_API virtual void starting() override;

    /**
     * @brief Stops the service.
     *
     * @throw core::tools::Failed
     */
    MODULE_FILTER_IMAGE_API virtual void stopping() override;

    /**
     * @brief Configures the service.
     *
     * @throw core::tools::Failed
     */
    MODULE_FILTER_IMAGE_API virtual void configuring() override;

    /**
     * @brief Compute the registration.
     *
     * @throw core::tools::Failed
     */
    MODULE_FILTER_IMAGE_API virtual void updating() override;

    /**
     * @brief Compute the registration by calling the `update` slot. The timestamp parameter is discarded.
     */
    MODULE_FILTER_IMAGE_API virtual void computeRegistration (core::HiResClock::HiResClockType timestamp) override;

private:
};

} // namespace sight::modules::filter::image

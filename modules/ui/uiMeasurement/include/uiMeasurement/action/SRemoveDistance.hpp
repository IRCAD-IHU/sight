/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "uiMeasurement/config.hpp"

#include <core/macros.hpp>

#include <data/Image.hpp>
#include <data/PointList.hpp>

#include <ui/base/IActionSrv.hpp>

namespace uiMeasurement
{

namespace action
{

/**
 * @brief This action removes distances from the image field.
 * Choose between removing a selected point or the last added point.
 *
 * @section Slots Slots
 * - \b removeLastDistance: Remove the last added point.
 *
 * @section XML XML Configuration
 * Example to remove a selected distance:
 * @code{.xml}
   <service type="::uiMeasurement::action::SRemoveDistance">
       <inout key="image" uid="..." />
   </service>
   @endcode
 *
 * Example to remove the last added distance:
 * @code{.xml}
    <service uid="SRemoveDistanceAct" type="::uiMeasurement::action::SRemoveDistance">
            <inout key="image" uid="${image}" />
    </service>

    <service uid="removeLastDistance" type="::modules::ui::base::action::SSlotCaller">
        <slots>
            <slot>SRemoveDistanceAct/removeLastDistance</slot>
        </slots>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [data::Image]: Image containing the distance field.
 */
class UIMEASUREMENT_CLASS_API SRemoveDistance final : public ::sight::ui::base::IActionSrv
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SRemoveDistance,  ui::base::IActionSrv)

    /// Initialize slots.
    UIMEASUREMENT_API SRemoveDistance() noexcept;

    /// Destroys the service.
    UIMEASUREMENT_API ~SRemoveDistance() noexcept override;

protected:

    /// Configures the action.
    UIMEASUREMENT_API void configuring() override;

    /// Starts the action.
    UIMEASUREMENT_API void starting() override;

    /// Removes a distance chosen from a gui dialog.
    UIMEASUREMENT_API void updating() override;

    /// Stops the action.
    UIMEASUREMENT_API void stopping() override;

private:

    /// Changes a distance value to a string.
    static std::string distanceToStr(double _dist);

    /**
     * @brief Opens a gui dialog to choose the distance to remove.
     * @param _image The image where the distance need to be removed.
     * @param _removeAll Is set to true if all distance need to be removed.
     * @return The distance to removed only if a specific distance is chosen, if all distances need to be removed, a
     * nullptr is return.
     */
    static data::PointList::sptr getDistanceToRemove(const data::Image::csptr _image, bool& _removeAll);

    /**
     * @brief Sends a signal when a distance is removed.
     * @param _image The image where the distance has been added.
     * @param _distance The distance value.
     */
    void notifyDeleteDistance(const data::Image::csptr& _image, const data::PointList::csptr& _distance) const;

    /// Removes the last added distance.
    void removeLastDistance();
};

} // namespace action.

} // namespace uiMeasurement.

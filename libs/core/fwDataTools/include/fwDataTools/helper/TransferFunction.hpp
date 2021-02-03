/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "fwDataTools/config.hpp"

#include <core/com/Connection.hpp>
#include <core/com/helper/SigSlotConnection.hpp>
#include <core/com/Slot.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransferFunction.hpp>

namespace fwDataTools
{

namespace helper
{

/**
 * @brief   Helpers for transfer function.
 *
 * @section Slots Slots
 *
 * One function must be implemented by the service managing the transfer function. This function will be
 * called when the transfer function points are modified. If the second function is implemented, it will
 * be called when the transfer function windowing is modified, else, the first function will also be called.
 */
class FWDATATOOLS_CLASS_API TransferFunction
{

public:

    fwCoreClassMacro(TransferFunction)

    /// Constructor.
    FWDATATOOLS_API TransferFunction(const std::function<void()>&);

    /// Constructor.
    FWDATATOOLS_API TransferFunction(const std::function<void()>&, const std::function<void(double, double)>&);

    /// Destructor. Do nothing.
    FWDATATOOLS_API virtual ~TransferFunction();

    /**
     * @brief Create and set the default transfer function.
     *
     * Create the image's transfer function pool if if does not exist and then create the defaut transfer function.
     * Set the current transfer function to the default grey-level if no transfer function was set (using
     * setTransferFunction() ).
     */
    FWDATATOOLS_API void createTransferFunction( ::fwData::Image::sptr image );

    /// Set the current TransferFunction
    FWDATATOOLS_API void setTransferFunction(const fwData::TransferFunction::sptr&);

    /**
     * @brief Sets the transfer function, creates one if _tf is null (@see createTransferFunction).
     *
     * Connects the TF's signals to the updateTFPoints and updateTFWindowing slots.
     */
    FWDATATOOLS_API void setOrCreateTF(const ::fwData::TransferFunction::sptr&, const fwData::Image::sptr&);

    /// Get the current transfer function
    FWDATATOOLS_API ::fwData::TransferFunction::sptr getTransferFunction() const;

    /// Install connections to listen TF modifications
    FWDATATOOLS_API void installTFConnections();

    /// Remove the TF connections
    FWDATATOOLS_API void removeTFConnections();

    FWDATATOOLS_API core::com::Connection getTFUpdateConnection() const;

    FWDATATOOLS_API core::com::Connection getTFWindowingConnection() const;

protected:

    /**
     * @name Connections to transfer function
     * @{
     */

    /// Slot: called when transfer function are modified
    FWDATATOOLS_API void updateTFPoints();

    /// Slot: called when transfer function windowing is modified
    FWDATATOOLS_API void updateTFWindowing(double, double);

    typedef core::com::Slot<void ()> UpdateTFPointsSlotType;

    typedef core::com::Slot<void (double, double)> UpdateTFWindowingSlotType;

    /// Slot called when transfer function points are modified
    UpdateTFPointsSlotType::sptr m_slotUpdateTFPoints;

    /// Slot called when transfer function windowing is modified
    UpdateTFWindowingSlotType::sptr m_slotUpdateTFWindowing;
    /**
     * @}
     */

private:

    /// Transfer function selection
    ::fwData::TransferFunction::wptr m_transferFunction;

    /// Connections to the transfer function
    core::com::helper::SigSlotConnection m_tfConnections;

    std::function<void()> m_updateTFPoints;

    std::function<void(double, double)> m_updateTFWindowing;

};

} //namespace helper

} //namespace fwDataTools

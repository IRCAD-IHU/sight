/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "videoTools/config.hpp"

#include <arData/MatrixTL.hpp>

#include <arServices/ISynchronizer.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/base.hpp>

#include <fwData/TransformationMatrix3D.hpp>

namespace videoTools
{
/**
 * @brief   SMatrixSynchronizer reads the content of a MatrixTL into a TransformationMatrix3D.
 */
class VIDEOTOOLS_CLASS_API SMatrixSynchronizer : public ::arServices::ISynchronizer
{

public:

    fwCoreServiceMacro(SMatrixSynchronizer, ::arServices::ISynchronizer)

    /**
     * @brief Constructor.
     */
    VIDEOTOOLS_API SMatrixSynchronizer() noexcept;

    /**
     * @brief Destructor.
     */
    VIDEOTOOLS_API virtual ~SMatrixSynchronizer() noexcept
    {
    }

    /// Slot definition
    VIDEOTOOLS_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_MATRIX_SLOT;
    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> UpdateMatrixSlotType;

protected:
    /**
     * @brief This method is used to configure the service.
     *
     * @code{.xml}
       <service impl="::videoTools::SMatrixSynchronizer" type="::arServices::ISynchronizer">
            <inout>Matrix</inout>
            <in>MatrixTL</in>
       </service>
     * @endcode
     *
     * @subsection In-Out In-Out
     * - \b TL [::arData::MatrixTL]:  Defines the uid of the MatrixTL containing the matrices.
     *
     */

    VIDEOTOOLS_API void configuring() override;

    /// This method is used to initialize the service.
    VIDEOTOOLS_API void starting() override;

    /// Does nothing.
    VIDEOTOOLS_API void stopping() override;

    /// Does nothing.
    VIDEOTOOLS_API void updating() override
    {
    }

private:

    /// slot to update the TransformationMatrix3D with the timeline buffer
    UpdateMatrixSlotType::sptr m_slotUpdateMatrix;
    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// This function fills the TransformationMatrix3D with the current buffer content of the MatrixTL
    void updateMatrix(::fwCore::HiResClock::HiResClockType timestamp);

};

} //namespace videoTools

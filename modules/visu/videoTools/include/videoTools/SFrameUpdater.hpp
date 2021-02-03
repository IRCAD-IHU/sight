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

#include <arData/FrameTL.hpp>

#include <fwCore/HiResClock.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/IController.hpp>

namespace videoTools
{

/**
 * @brief   Service used to update frame and trigger render once timelines have been updated.
 *
 * @section Signals Signals
 * - \b renderRequested(): Emitted when the frame has been updated.
 *
 * @section Slots Slots
 * - \b updateFrame(::fwCore::HiResClock::HiResClockType): Called to extract the frame at the given timestamp.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::videoTools::SFrameUpdater">
            <in key="frameTL" uid="..." autoConnect="yes" />
            <inout key="frame" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b key1 [::arData::FrameTL]: timeline from which we synchronize images.
 * @subsection In-Out In-Out:
 * - \b key2 [::fwData::Image]: frame where to extract a buffer of a timeline.
 */
class VIDEOTOOLS_CLASS_API SFrameUpdater : public ::fwServices::IController
{
public:

    fwCoreServiceMacro(SFrameUpdater, ::fwServices::IController)

    static const ::fwCom::Slots::SlotKeyType s_UPDATE_FRAME_SLOT;

    /// Type of signal m_sigRenderRequested
    typedef ::fwCom::Signal< void () > RenderRequestedSignalType;

    /// Key in m_signals map of signal m_sigObjectModified
    static const ::fwCom::Signals::SignalKeyType s_RENDER_REQUESTED_SIG;

    /// Constructor
    VIDEOTOOLS_API SFrameUpdater() noexcept;

    /// Destructorin
    VIDEOTOOLS_API virtual ~SFrameUpdater() noexcept;

protected:

    /// Starting method. Initialize timer.
    VIDEOTOOLS_API virtual void starting() override;

    /// Configuring method.
    VIDEOTOOLS_API virtual void configuring() override;

    /// Stopping method.
    VIDEOTOOLS_API virtual void stopping() override;

    /// Updating method. Deforms the mesh
    VIDEOTOOLS_API virtual void updating() override;

    /// Update frame slots
    VIDEOTOOLS_API virtual void updateFrame( ::fwCore::HiResClock::HiResClockType timestamp );

    /// Update the image according to the frame timeline.
    VIDEOTOOLS_API virtual void updateImage();

    /// Request Render
    VIDEOTOOLS_API void requestRender();

    /// Reset the last timestamp when the timeline is cleared
    VIDEOTOOLS_API void resetTimeline();

private:

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connections
     *
     * Connect ::arData::TimeLine::s_OBJECT_PUSHED_SIG to s_UPDATE_FRAME_SLOT
     * Connect ::arData::TimeLine::s_CLEARED_SIG to s_RESET_TIMELINE_SLOT
     */
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Frame timeline
    ::arData::FrameTL::csptr m_frameTL;

    /// Image
    ::fwData::Image::sptr m_image;

    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// Hight resolution timer to log information about computing function time
    ::fwCore::HiResTimer m_hiRestimer;

    bool m_imageInitialized;
};

} //namespace videoTools

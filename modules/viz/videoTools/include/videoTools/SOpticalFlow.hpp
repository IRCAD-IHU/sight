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

#include "videoTools/config.hpp"

#include <services/IController.hpp>

#include <opencv2/core.hpp>

namespace videoTools
{

/**
 * @brief Service used to compute optical flow (Lukas-Kanade) to detect if camera is moving or not.
 *  \b Note: this service is not detecting if something is moving in a video,
 * due to internal parameters it only detects if camera is moving globally.
 *
 * @section Signals Signals
 * - \b cameraMoved: send when camera has moved.
 *  -\b cameraRemained: send when camera is detected as stable.
 *
 * @section Slots Slots
 * - \b updating: called when a new frame is pushed into the timeline. Detected if camera has moved or not.

 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="::videoTools::SOpticalFlow" worker="ofWorker  >
        <in key="timeline" uid="..." autoConnect="yes" />
        <config latency="333" scaleFactor="3.6" />
     </service>
   @endcode
 * @subsection Input Input:
 * - \b timeline [data::FrameTL]: timeline on which optical flow will be computed.
 * @subsection Configuration Configuration:
 * - \b latency (optionnal): There is no need to process every frames coming in.
 * Use `latency` to wait between two consecutive frame. Ex: if set to 333 (in ms), it will compute ~1/10 frames at 30fps
 * (Default: 333 ms).
 * Usually you don't need to change the value.
 * - \b scaleFactor (optionnal): if image is > 640 x 480 the scaleFactor is applied to downscale image
 * to keep a good balance between computation time and feature tracking quality (default 3,6),
 *  usually you don't need to change the value.
 */
class VIDEOTOOLS_CLASS_API SOpticalFlow : public services::IController
{
public:
    fwCoreServiceMacro(SOpticalFlow, services::IController)

    typedef core::com::Signal<void ()> MotionSignalType;
    typedef core::com::Signal<void ()> NoMotionSignalType;

    /// Constructor
    VIDEOTOOLS_CLASS_API SOpticalFlow() noexcept;

    /// Destructor
    VIDEOTOOLS_CLASS_API ~SOpticalFlow() noexcept override;

    /// Connects ::arData:FrameTL::s_OBJECT_PUSHED_SIG to s_UPDATE_SLOT
    VIDEOTOOLS_CLASS_API services::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Does nothing.
    VIDEOTOOLS_CLASS_API void configuring() override;

    /// Does nothing.
    VIDEOTOOLS_CLASS_API void starting() override;

    /// Computes Optical flow.
    VIDEOTOOLS_CLASS_API void updating() override;

    /// Does nothing.
    VIDEOTOOLS_CLASS_API void stopping() override;

private:
    /// Handles concurrency.
    std::mutex m_mainMutex;
    /// Signal send when motion is detected.
    MotionSignalType::sptr m_motionSignal;
    /// Signal send when no motion is detected.
    NoMotionSignalType::sptr m_noMotionSignal;
    /// Stores last image.
    ::cv::Mat m_lastGrayImg;
    /// Stores last corners.
    ::cv::Mat m_lastCorners;
    /// Waiting time between to frames.
    unsigned int m_latency;
    /// Factor of re-scale: resize image to keep good balance between computation time and feature tracking quality.
    float m_imageScaleFactor;
    /// Optical flow can only be computed if a frame is already present (see: m_lastGrayImg).
    /// True if it is, False otherwise
    bool m_initialization;
    /// Stores last behavior (true if motion, false otherwise).
    bool m_motion;
    /// Stores last processed frame timestamp.
    core::HiResClock::HiResClockType m_lastTimestamp;
};

} //namespace videoTools

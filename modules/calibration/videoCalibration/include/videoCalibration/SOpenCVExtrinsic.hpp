/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "videoCalibration/config.hpp"

#include <services/ICalibration.hpp>

namespace videoCalibration
{
/**
 * @brief   SOpenCVExtrinsic service that computes extrinsic calibration with openCV.
 *
 * @section Slots Slots
 * - \b updateChessboardSize() : Received when the chessboard preferences parameters change.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::videoCalibration::SOpenCVExtrinsic">
            <in key="calibrationInfo1" uid="..." />
            <in key="calibrationInfo2" uid="..." />
            <inout key="cameraSeries" uid="..." />
            <out key="matrix" uid="..." />
            <camIndex>...</camIndex>
            <board width="CHESSBOARD_WIDTH" height="CHESSBOARD_HEIGHT" squareSize="CHESSBOARD_SQUARE_SIZE" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b calibrationInfo1 [data::CalibrationInfo]: Data of the first camera used to compute the calibration.
 * - \b calibrationInfo2 [data::CalibrationInfo]: Data of the second camera used to compute the calibration.
 * @subsection In-Out In-Out:
 * - \b camera [data::CameraSeries]: Output calibration.
 *  * @subsection Output Output:
 * - \b matrix [data::TransformationMatrix3D]: Extrinsic matrix (for export purpose) (optionnal).
 * @subsection Configuration Configuration:
 * - \b camIndex (optional, default: 1): index of the camera in \b cameraSeries used to compute extrinsic matrix
 *      (from camera[0] to camera[index]).
 * - \b board : preference key to retrieve the number of square in 2 dimensions of the chessboard.
 */
class VIDEOCALIBRATION_CLASS_API SOpenCVExtrinsic : public services::ICalibration
{
public:
    fwCoreServiceMacro(SOpenCVExtrinsic, services::ICalibration)

    /// Double changed signal type
    typedef core::com::Signal< void (double) > ErrorComputedSignalType;

    /// Constructor.
    VIDEOCALIBRATION_API SOpenCVExtrinsic() noexcept;

    /// Destructor.
    VIDEOCALIBRATION_API virtual ~SOpenCVExtrinsic() noexcept;

protected:

    /// Configures the service.
    VIDEOCALIBRATION_API void configuring() override;

    /// Does nothing.
    VIDEOCALIBRATION_API void starting() override;

    /// Calls stopping and starting.
    VIDEOCALIBRATION_API void swapping() override;

    /// Computes intrinsic calibration
    VIDEOCALIBRATION_API void updating() override;

    /// Removes connections
    VIDEOCALIBRATION_API void stopping() override;

private:

    /**
     * @brief SLOT: update the chessboard size.
     */
    void updateChessboardSize();

    /// FwId of the first calibrationInfo
    std::string m_calibrationInfo1ID;

    /// FwId of the second calibrationInfo
    std::string m_calibrationInfo2ID;

    /// Preference key to retrieve width of the chessboard used for calibration
    std::string m_widthKey;

    /// Preference key to retrieve height of the chessboard used for calibration
    std::string m_heightKey;

    /// Preference key to retrieve size of the chessboard'square used for calibration
    std::string m_squareSizeKey;

    /// Width of the chessboard used for calibration
    unsigned int m_width;

    /// Height of the chessboard used for calibration
    unsigned int m_height;

    /// Size of the chessboard'square used for calibration
    float m_squareSize;

    /// Index of the camera in cameraSeries used to compute extrinsic matrix (from camera[0] to camera[index]).
    size_t m_camIndex;
};
} // namespace videoCalibration

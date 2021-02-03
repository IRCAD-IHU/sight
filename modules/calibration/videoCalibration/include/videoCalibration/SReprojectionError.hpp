/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <data/MarkerMap.hpp>

#include <fwServices/IController.hpp>

#include <opencv2/core.hpp>

namespace videoCalibration
{
/**
 * @brief   SReprojectionError Class used to compute the mean error of reprojection between 3d object points
 * and 2d image points.
 *
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="::videoCalibration::SReprojectionError">
        <in key="matrixTL" uid="..."/>
        <in key="markerTL" uid="..."/>
        <in key="camera" uid="..."/>
        <in key="extrinsic" uid="..."/>
        <inout key="frameTL" uid="..." />
        <out key="error" uid="..." />
        <patternWidth>80</patternWidth>
     </service>

     or

     <service uid="..." type="::registrationCV::SReprojectionError">
         <in group="matrix">
             <key uid="..." />
             <key uid="..." />
             <key uid="..." />
             <key uid="..." />
         </in>
         <in key="markerMap" uid="..." />
         <in key="camera" uid="..."/>
         <in key="extrinsic" uid="..." />
         <inout key="frame" uid="..." />
         <patternWidth>80</patternWidth>
     </service>
   @endcode
 * @subsection Input Input
 * - \b markerTL [data::MarkerTL]: timeline for markers.
 * - \b markerMap [data::MarkerMap]: markers map list.
 * - \b camera [data::Camera]: calibrated cameras.
 * - \b extrinsic [data::TransformationMatrix3D]: extrinsic matrix, only used if you have two cameras configured.
 * - \b matrixTL [data::MatrixTL]: timeline of 3D transformation matrices.
 * - \b matrix [data::TransformationMatrix3D]: list of matrices related to the markers. The marker's id must be
 * specified using the \b id tag to be found in the marker map.
 * @subsection InOut InOut
 * - \b frameTL [data::FrameTL] : frame timeline used to draw reprojected points (optional)
 * - \b frame [data::Image]: video frame.
 * @subsection Output Output
 * - \b error [data::Float] : computed error
 * @subsection Configuration Configuration
 * - \b patternWidth : width of the tag.
 */
class VIDEOCALIBRATION_CLASS_API SReprojectionError : public ::fwServices::IController
{
public:
    fwCoreServiceMacro(SReprojectionError, fwServices::IController)

    /// Double changed signal type
    typedef core::com::Signal< void (double) > ErrorComputedSignalType;

    VIDEOCALIBRATION_API static const core::com::Slots::SlotKeyType s_COMPUTE_SLOT;

    VIDEOCALIBRATION_API static const core::com::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    VIDEOCALIBRATION_API static const core::com::Slots::SlotKeyType s_SET_COLOR_PARAMETER_SLOT;

    ///Constructor
    VIDEOCALIBRATION_API SReprojectionError();

    ///Destructor
    VIDEOCALIBRATION_API ~SReprojectionError();

    /// Connect MatrixTL::s_OBJECT_PUSHED_SIG to s_COMPUTE_SLOT
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:
    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    VIDEOCALIBRATION_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    VIDEOCALIBRATION_API void starting() override;

    /**
     * @brief Updating method : This method is used to restart the service.
     */
    VIDEOCALIBRATION_API void updating() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    VIDEOCALIBRATION_API void stopping() override;

private:

    void compute(core::HiResClock::HiResClockType timestamp);

    ///Slot called when a color value is changed
    void setColorParameter(std::array< std::uint8_t, 4 > _val, std::string _key);
    ///Slot called when a boolean value is changed
    void setBoolParameter(bool _val, std::string _key);

    /// Last timestamp
    core::HiResClock::HiResClockType m_lastTimestamp;

    /// Marker pattern width.
    double m_patternWidth;

    /// 3D object points
    std::vector< ::cv::Point3f > m_objectPoints;
    /// Camera Matrix (fx, fy, cx, cy)
    ::cv::Mat m_cameraMatrix;
    ///Distorsion coefficient
    ::cv::Mat m_distorsionCoef;
    /// Color of the reprojection circle
    ::cv::Scalar m_cvColor;
    /// if true: display circle centered at reprojection point.
    bool m_display;
    /// extrinsic matrix (can be identity)
    ::cv::Mat m_extrinsic;

    /// List of tags associated with each input matrix
    std::vector< data::MarkerMap::KeyType> m_matricesTag;
};

}//namespace videoCalibration

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
#include "registrationCV/config.hpp"

#include <core/HiResClock.hpp>
#include <core/mt/types.hpp>

#include <data/MarkerMap.hpp>

#include <services/IRegisterer.hpp>
#include <services/macros.hpp>

#include <opencv2/core.hpp>

namespace registrationCV
{

/**
 * @brief   SPoseFrom2d Class used to compute the 3d pose of a object using 2d points.
 *
 * @section Slots Slots
 * - \b computeRegistration(core::HiResClock::HiResClockType timestamp) : computes the registration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="::registrationCV::SPoseFrom2d">
         <in group="markerTL" autoConnect="yes">
             <key uid="..." />
             <key uid="..." />
         </in>
         <in group="camera">
             <key uid="..." />
             <key uid="..." />
         </in>
         <in key="extrinsic" uid="..." />
         <inout key="matrixTL" uid="..." />
         <inout key="pointList" uid="..." />
         <patternWidth>80</patternWidth>
     </service>

     or

     <service uid="..." type="::registrationCV::SPoseFrom2d">
         <in group="markerMap" autoConnect="yes">
             <key uid="..." />
             <key uid="..." />
         </in>
         <in group="camera">
             <key uid="..." />
             <key uid="..." />
         </in>
         <in key="extrinsic" uid="..." />
         <in group="matrix">
             <key uid="..." id="101"/>
             <key uid="..." id="102"/>
             <key uid="..." id="103"/>
             <key uid="..." id="104"/>
         </in>
         <patternWidth>80</patternWidth>
     </service>
   @endcode
 * @subsection Input Input
 * - \b markerTL [data::MarkerTL]: timeline for markers.
 * - \b markerMap [data::MarkerMap]: markers map list.
 * - \b camera [data::Camera]: calibrated cameras.
 * - \b extrinsic [data::TransformationMatrix3D]: extrinsic matrix, only used if you have two cameras configured.
 * @subsection In-Out In-Out
 * - \b matrixTL [data::MatrixTL]: timeline of 3D transformation matrices.
 * - \b matrix [data::TransformationMatrix3D]: list of matrices related to the markers. The marker's id must be
 * specified using the \b id tag to be found in the marker map.
 * - \b pointList [data::PointList] (optional): list of points corresponding to the model.
 * @subsection Configuration Configuration
 * - \b patternWidth : width of the tag.
 */
class REGISTRATIONCV_CLASS_API SPoseFrom2d : public services::IRegisterer
{
public:
    fwCoreServiceMacro(SPoseFrom2d, services::IRegisterer)

    typedef std::vector<std::string> VectKeyType;

    /**
     * @brief Constructor.
     */
    REGISTRATIONCV_API SPoseFrom2d() noexcept;

    /**
     * @brief Destructor.
     */
    REGISTRATIONCV_API virtual ~SPoseFrom2d() noexcept;

    /// Connect MarkerTL::s_OBJECT_PUSHED_SIG to s_REGISTER_SLOT
    services::IService::KeyConnectionsMap getAutoConnections() const override;

protected:
    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    REGISTRATIONCV_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    REGISTRATIONCV_API void starting() override;

    /**
     * @brief Updating method : This method is used to update the service.
     */
    REGISTRATIONCV_API void updating() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    REGISTRATIONCV_API void stopping() override;

    /// Register matrix slot
    void computeRegistration(core::HiResClock::HiResClockType timestamp) override;

private:

    /**
     * @brief The Marker struct:
     * to handle marker object, with 4 corners 2D, and optionnaly a centroid
     */
    struct Marker
    {
        std::vector< ::cv::Point2f > corners2D;
        ::cv::Point3f centroid;
    };

    /**
     * @brief The Camera struct : to handle intrinsic parameters and distorsion coeficient.
     */
    struct Camera
    {
        ::cv::Size imageSize;
        ::cv::Mat intrinsicMat;
        ::cv::Mat distCoef;
    };
    /**
     * @brief The Extrinsic struct : to handle several format of extrinsic matrix
     */
    struct Extrinsic
    {
        ::cv::Mat rotation;
        ::cv::Mat translation;
        ::cv::Mat Matrix4x4;
    };

    /// Initialize cameras
    void initialize();

    /**
     * @brief : Compute the camera position from a maker detected on two separated view
     * @param : Marker points in each view
     *
     **/
    const ::cv::Matx44f cameraPoseFromStereo(const Marker& _markerCam1, const Marker& _markerCam2) const;

    /**
     * @brief :Compute the camera position from a marker detected in one view
     **/
    const ::cv::Matx44f cameraPoseFromMono(const Marker& _markerCam1) const;

    /// Last timestamp
    core::HiResClock::HiResClockType m_lastTimestamp;

    /// Marker pattern width.
    double m_patternWidth;

    /// True if the service is initialized (timelines and cameras)
    bool m_isInitialized;

    /// 3d model
    std::vector< ::cv::Point3f > m_3dModel;

    /// std::vector of cameras
    std::vector < Camera > m_cameras;

    /// Extrinsic matrix
    Extrinsic m_extrinsic;

    /// List of tags associated with each inout matrix
    std::vector< data::MarkerMap::KeyType> m_matricesTag;
};

} // namespace registrationCV

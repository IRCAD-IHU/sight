/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "io/session/config.hpp"
#include "io/session/detail/Helper.hpp"
#include "io/session/detail/Series.hpp"

#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>

namespace sight::io::session
{

namespace detail::CameraSeries
{

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto& cameraSeries = Helper::safeCast<data::CameraSeries>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::CameraSeries>(tree, 1);

    // Since CameraSeries inherits from Series, we could use SeriesSerializer
    Series::serialize(archive, tree, cameraSeries, children, password);

    // Serialize the children camera
    for(size_t index = 0, end = cameraSeries->getNumberOfCameras() ; index < end ; ++index)
    {
        children[data::Camera::classname() + std::to_string(index)] = cameraSeries->getCamera(index);
    }
}

//------------------------------------------------------------------------------

inline static data::CameraSeries::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    const auto& cameraSeries = Helper::safeCast<data::CameraSeries>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::CameraSeries>(tree, 0, 1);

    // Since CameraSeries inherits from Series, we could use SeriesDeserializer
    Series::deserialize(archive, tree, children, cameraSeries, password);

    // Deserialize children
    // Clearing is required in case the object is reused
    cameraSeries->clearCameras();

    for(size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& cameraIt = children.find(data::Camera::classname() + std::to_string(index));

        // If we didn't found a matching camera, exit the loop
        if(cameraIt == children.cend())
        {
            break;
        }

        const auto& camera = Helper::safeCast<data::Camera>(cameraIt->second);
        cameraSeries->addCamera(camera);
    }

    return cameraSeries;
}

} // namespace detail::CameraSeries

} // namespace sight::io
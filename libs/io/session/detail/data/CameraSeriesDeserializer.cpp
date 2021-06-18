/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "CameraSeriesDeserializer.hpp"

#include "SeriesDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/CameraSeries.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr CameraSeriesDeserializer::deserialize(
    const zip::ArchiveReader::sptr& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string& password
) const
{
    // Create or reuse the object
    const auto& cameraSeries = safeCast<sight::data::CameraSeries>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::CameraSeries>(tree, 0, 1);

    // Since CameraSeries inherits from Series, we could use SeriesDeserializer
    const SeriesDeserializer seriesDeserializer;
    seriesDeserializer.deserialize(archive, tree, children, cameraSeries, password);

    // Deserialize children
    for(size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& cameraIt = children.find(sight::data::Camera::classname() + std::to_string(index));

        // If we didn't found a matching camera, exit the loop
        if(cameraIt == children.cend())
        {
            break;
        }

        const auto& camera = safeCast<sight::data::Camera>(cameraIt->second);
        cameraSeries->addCamera(camera);
    }

    return cameraSeries;
}

} // detail::data

} // namespace sight::io::session

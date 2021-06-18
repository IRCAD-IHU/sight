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

#include "CalibrationInfoDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/CalibrationInfo.hpp>
#include <data/Point.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr CalibrationInfoDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& calibrationInfo = safeCast<sight::data::CalibrationInfo>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::CalibrationInfo>(tree, 0, 1);

    // Deserialize children
    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        // Find the image ad the associated pointList
        const std::string& indexString = std::to_string(index);
        const auto& imageIt            = children.find(sight::data::Image::classname() + indexString);
        const auto& pointListIt        = children.find(sight::data::PointList::classname() + indexString);

        // If we didn't found a matching image and pointList, exit the loop
        if(imageIt == children.cend() || pointListIt == children.cend())
        {
            break;
        }

        const auto& image     = safeCast<sight::data::Image>(imageIt->second);
        const auto& pointList = safeCast<sight::data::PointList>(pointListIt->second);

        calibrationInfo->addRecord(image, pointList);
    }

    return calibrationInfo;
}

} // detail::data

} // namespace sight::io::session

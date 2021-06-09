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
    const zip::ArchiveReader::sptr& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string& password
) const
{
    // Create or reuse the object
    const auto& calibrationInfo =
        object ? sight::data::CalibrationInfo::dynamicCast(object) : sight::data::CalibrationInfo::New();

    SIGHT_ASSERT(
        "Object '" << calibrationInfo->getClassname() << "' is not a '" << sight::data::CalibrationInfo::classname() << "'",
        calibrationInfo
    );

    // Check version number. Not mandatory, but could help for future release
    const int version = tree.get<int>("version", 0);
    SIGHT_THROW_IF(
        CalibrationInfoDeserializer::classname() << " is not implemented for version '" << version << "'.",
        version > 1
    );

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

        const auto& image = sight::data::Image::dynamicCast(imageIt->second);
        SIGHT_ASSERT(
            "Not a " << sight::data::Image::classname() << ": " << imageIt->second->getClassname(),
            image
        );

        const auto& pointList = sight::data::PointList::dynamicCast(pointListIt->second);
        SIGHT_ASSERT(
            "Not a " << sight::data::PointList::classname() << ": " << pointListIt->second->getClassname(),
            pointList
        );

        calibrationInfo->addRecord(image, pointList);
    }

    return calibrationInfo;
}

} // detail::data

} // namespace sight::io::session

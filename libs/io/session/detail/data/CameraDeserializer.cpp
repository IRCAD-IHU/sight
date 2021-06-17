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

#include "CameraDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/Camera.hpp>
#include <data/Point.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr CameraDeserializer::deserialize(
    const zip::ArchiveReader::sptr& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string& password
) const
{
    // Create or reuse the object
    const auto& camera =
        object ? sight::data::Camera::dynamicCast(object) : sight::data::Camera::New();

    SIGHT_ASSERT(
        "Object '" << camera->getClassname() << "' is not a '" << sight::data::Camera::classname() << "'",
        camera
    );

    // Check version number. Not mandatory, but could help for future release
    const int version = tree.get<int>("version", 0);
    SIGHT_THROW_IF(
        CameraDeserializer::classname() << " is not implemented for version '" << version << "'.",
        version > 1
    );

    camera->setWidth(tree.get<size_t>("Width"));
    camera->setHeight(tree.get<size_t>("Height"));

    camera->setFx(tree.get<double>("Fx"));
    camera->setFy(tree.get<double>("Fy"));
    camera->setCx(tree.get<double>("Cx"));
    camera->setCy(tree.get<double>("Cy"));

    camera->setDistortionCoefficient(
        tree.get<double>("K1"),
        tree.get<double>("K2"),
        tree.get<double>("P1"),
        tree.get<double>("P2"),
        tree.get<double>("K3")
    );

    camera->setSkew(tree.get<double>("Skew"));

    camera->setIsCalibrated(tree.get<bool>("IsCalibrated"));
    camera->setDescription(this->readFromTree(tree, "Description"));
    camera->setCameraID(this->readFromTree(tree, "CameraID"));
    camera->setMaximumFrameRate(tree.get<float>("MaximumFrameRate"));
    camera->setPixelFormat(static_cast<sight::data::Camera::PixelFormat>(tree.get<int>("PixelFormat")));
    camera->setVideoFile(this->readFromTree(tree, "VideoFile"));
    camera->setStreamUrl(this->readFromTree(tree, "StreamUrl"));
    camera->setCameraSource(static_cast<sight::data::Camera::SourceType>(tree.get<int>("CameraSource")));
    camera->setScale(tree.get<double>("Scale"));

    return camera;
}

} // detail::data

} // namespace sight::io::session

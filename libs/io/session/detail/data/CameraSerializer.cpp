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

#include "CameraSerializer.hpp"

#include <data/Camera.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void CameraSerializer::serialize(
    const zip::ArchiveWriter::sptr&,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>&,
    const core::crypto::secure_string&
) const
{
    const auto& camera = IDataSerializer::safeCast<sight::data::Camera>(object);

    // Add a version number. Not mandatory, but could help for future release
    IDataSerializer::writeVersion<sight::data::Camera>(tree, 1);

    tree.put("Width", camera->getWidth());
    tree.put("Height", camera->getHeight());

    tree.put("Fx", camera->getFx());
    tree.put("Fy", camera->getFy());
    tree.put("Cx", camera->getCx());
    tree.put("Cy", camera->getCy());

    const auto& distortionCoefficient = camera->getDistortionCoefficient();
    tree.put("K1", distortionCoefficient[0]);
    tree.put("K2", distortionCoefficient[1]);
    tree.put("P1", distortionCoefficient[2]);
    tree.put("P2", distortionCoefficient[3]);
    tree.put("K3", distortionCoefficient[4]);

    tree.put("Skew", camera->getSkew());

    tree.put("IsCalibrated", camera->getIsCalibrated());
    this->writeString(tree, "Description", camera->getDescription());
    this->writeString(tree, "CameraID", camera->getCameraID());
    tree.put("MaximumFrameRate", camera->getMaximumFrameRate());
    tree.put("PixelFormat", camera->getPixelFormat());
    this->writeString(tree, "VideoFile", camera->getVideoFile().string());
    this->writeString(tree, "StreamUrl", camera->getStreamUrl());
    tree.put("CameraSource", camera->getCameraSource());
    tree.put("Scale", camera->getScale());
}

} // detail::data

} // namespace sight::io::session

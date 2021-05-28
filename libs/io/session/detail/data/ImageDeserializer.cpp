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

#include "ImageDeserializer.hpp"

#include "io/vtk/vtk.hpp"

#include <core/exceptionmacros.hpp>

#include <data/Image.hpp>

#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataReader.h>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr ImageDeserializer::deserialize(
    const zip::ArchiveReader::sptr& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string& password
) const
{
    // Create or reuse the object
    const auto& image = object ? sight::data::Image::dynamicCast(object) : sight::data::Image::New();

    SIGHT_ASSERT(
        "Object '" << image->getClassname() << "' is not a '" << sight::data::Image::classname() << "'",
        image
    );

    // Check version number. Not mandatory, but could help for future release
    const int version = tree.get<int>("version", 0);
    SIGHT_THROW_IF(
        ImageDeserializer::classname() << " is not implemented for version '" << version << "'.",
        version > 1
    );

    // Create the istream from the input file inside the archive
    const auto& uuid    = tree.get<std::string>("uuid");
    const auto& istream = archive->openFile(
        std::filesystem::path(uuid + "/image.vti"),
        password
    );

    // "Convert" it to a string
    const std::string content {std::istreambuf_iterator<char>(*istream), std::istreambuf_iterator<char>()};

    // Create the vtk reader
    const auto& vtkReader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    vtkReader->ReadFromInputStringOn();
    vtkReader->SetInputString(content);
    vtkReader->Update();

    // Convert from VTK
    io::vtk::fromVTKImage(vtkReader->GetOutput(), image);

    return image;
}

} // detail::data

} // namespace sight::io::session

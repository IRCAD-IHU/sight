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

#include "SessionDeserializer.hpp"

#include "data/ActivitySeriesDeserializer.hpp"
#include "data/ArrayDeserializer.hpp"
#include "data/CalibrationInfoDeserializer.hpp"
#include "data/CameraDeserializer.hpp"
#include "data/CameraSeriesDeserializer.hpp"
#include "data/ColorDeserializer.hpp"
#include "data/CompositeDeserializer.hpp"
#include "data/EdgeDeserializer.hpp"
#include "data/EquipmentDeserializer.hpp"
#include "data/GenericDeserializer.hpp"
#include "data/GraphDeserializer.hpp"
#include "data/HistogramDeserializer.hpp"
#include "data/ImageDeserializer.hpp"
#include "data/LandmarksDeserializer.hpp"
#include "data/LineDeserializer.hpp"
#include "data/ListDeserializer.hpp"
#include "data/MaterialDeserializer.hpp"
#include "data/Matrix4Deserializer.hpp"
#include "data/MeshDeserializer.hpp"
#include "data/NodeDeserializer.hpp"
#include "data/PatientDeserializer.hpp"
#include "data/PlaneDeserializer.hpp"
#include "data/PlaneListDeserializer.hpp"
#include "data/PointDeserializer.hpp"
#include "data/PointListDeserializer.hpp"
#include "data/PortDeserializer.hpp"
#include "data/ProcessObjectDeserializer.hpp"
#include "data/SeriesDeserializer.hpp"
#include "data/StringDeserializer.hpp"
#include "data/StudyDeserializer.hpp"
#include "data/VectorDeserializer.hpp"

#include <data/ActivitySeries.hpp>
#include <data/Array.hpp>
#include <data/Boolean.hpp>
#include <data/CalibrationInfo.hpp>
#include <data/Camera.hpp>
#include <data/CameraSeries.hpp>
#include <data/Color.hpp>
#include <data/Composite.hpp>
#include <data/Edge.hpp>
#include <data/Equipment.hpp>
#include <data/Float.hpp>
#include <data/Graph.hpp>
#include <data/Histogram.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/Landmarks.hpp>
#include <data/Line.hpp>
#include <data/List.hpp>
#include <data/Material.hpp>
#include <data/Matrix4.hpp>
#include <data/Mesh.hpp>
#include <data/mt/locked_ptr.hpp>
#include <data/Node.hpp>
#include <data/Patient.hpp>
#include <data/Plane.hpp>
#include <data/PlaneList.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/Port.hpp>
#include <data/ProcessObject.hpp>
#include <data/Series.hpp>
#include <data/String.hpp>
#include <data/Study.hpp>
#include <data/Vector.hpp>

#include <io/zip/ArchiveReader.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <memory>

namespace sight::io::session
{

namespace detail
{

// Serializer registry
// No concurrency protection as the map is statically initialized
static const std::unordered_map<std::string, std::function<data::IDataDeserializer::cuptr(void)> > s_deserializers = {
    {sight::data::Boolean::classname(), &std::make_unique<data::GenericDeserializer<sight::data::Boolean> >},
    {sight::data::Integer::classname(), &std::make_unique<data::GenericDeserializer<sight::data::Integer> >},
    {sight::data::Float::classname(), &std::make_unique<data::GenericDeserializer<sight::data::Float> >},
    {sight::data::String::classname(), &std::make_unique<data::StringDeserializer>},
    {sight::data::Composite::classname(), &std::make_unique<data::CompositeDeserializer>},
    {sight::data::Mesh::classname(), &std::make_unique<data::MeshDeserializer>},
    {sight::data::Equipment::classname(), &std::make_unique<data::EquipmentDeserializer>},
    {sight::data::Patient::classname(), &std::make_unique<data::PatientDeserializer>},
    {sight::data::Study::classname(), &std::make_unique<data::StudyDeserializer>},
    {sight::data::Series::classname(), &std::make_unique<data::SeriesDeserializer>},
    {sight::data::ActivitySeries::classname(), &std::make_unique<data::ActivitySeriesDeserializer>},
    {sight::data::Array::classname(), &std::make_unique<data::ArrayDeserializer>},
    {sight::data::Image::classname(), &std::make_unique<data::ImageDeserializer>},
    {sight::data::Vector::classname(), &std::make_unique<data::VectorDeserializer>},
    {sight::data::Point::classname(), &std::make_unique<data::PointDeserializer>},
    {sight::data::PointList::classname(), &std::make_unique<data::PointListDeserializer>},
    {sight::data::CalibrationInfo::classname(), &std::make_unique<data::CalibrationInfoDeserializer>},
    {sight::data::Camera::classname(), &std::make_unique<data::CameraDeserializer>},
    {sight::data::CameraSeries::classname(), &std::make_unique<data::CameraSeriesDeserializer>},
    {sight::data::Color::classname(), &std::make_unique<data::ColorDeserializer>},
    {sight::data::Edge::classname(), &std::make_unique<data::EdgeDeserializer>},
    {sight::data::Port::classname(), &std::make_unique<data::PortDeserializer>},
    {sight::data::Node::classname(), &std::make_unique<data::NodeDeserializer>},
    {sight::data::Graph::classname(), &std::make_unique<data::GraphDeserializer>},
    {sight::data::Histogram::classname(), &std::make_unique<data::HistogramDeserializer>},
    {sight::data::Landmarks::classname(), &std::make_unique<data::LandmarksDeserializer>},
    {sight::data::Line::classname(), &std::make_unique<data::LineDeserializer>},
    {sight::data::List::classname(), &std::make_unique<data::ListDeserializer>},
    {sight::data::Material::classname(), &std::make_unique<data::MaterialDeserializer>},
    {sight::data::Matrix4::classname(), &std::make_unique<data::Matrix4Deserializer>},
    {sight::data::Plane::classname(), &std::make_unique<data::PlaneDeserializer>},
    {sight::data::PlaneList::classname(), &std::make_unique<data::PlaneListDeserializer>},
    {sight::data::ProcessObject::classname(), &std::make_unique<data::ProcessObjectDeserializer>}
};

// Return a writer from a data object class name
inline static data::IDataDeserializer::cuptr findDeserializer(const std::string& classname)
{
    const auto& it = s_deserializers.find(classname);

    if(it != s_deserializers.cend())
    {
        // Return the found writer
        return it->second();
    }

    // Not found return empty one
    return data::IDataDeserializer::uptr();
}

/// Deserializes recursively an initialized archive to a data::Object using an opened property tree
/// @param cache object cache
/// @param archive initialized archive
/// @param tree property tree used to retrieve object index
/// @param password password to use for optional encryption. Empty password means no encryption
inline static sight::data::Object::sptr deepDeserialize(
    std::map<std::string, sight::data::Object::sptr>& cache,
    const zip::ArchiveReader::sptr& archive,
    const boost::property_tree::ptree& tree,
    const core::crypto::secure_string& password
)
{
    // First check the cache
    const auto& treeIt     = tree.begin();
    const auto& objectTree = treeIt->second;
    const auto& uuid       = objectTree.get<std::string>("uuid");
    const auto& objectIt   = cache.find(uuid);

    if(objectIt != cache.cend())
    {
        return objectIt->second;
    }
    else
    {
        // Find the serializer using the classname
        const auto& classname    = treeIt->first;
        const auto& deserializer = findDeserializer(classname);

        SIGHT_ASSERT(
            "There is no deserializer registered for class '" << classname << "'.",
            deserializer
        );

        // Try to reuse existing rather than create new one
        // Existing object will be overwritten
        auto object = sight::data::Object::dynamicCast(sight::data::Object::fromUUID(uuid));

        // Lock for writing (it will do nothing if object is null)
        sight::data::mt::locked_ptr<sight::data::Object> object_guard(object);

        if(!object)
        {
            // Create the new object so we can safely deserialize child
            object = sight::data::factory::New(classname);
            object->setUUID(uuid);
        }

        // Store the object in cache for later use and to allow circular reference
        cache[uuid] = object;

        // Construct children map, if needed
        std::map<std::string, sight::data::Object::sptr> children;

        const auto& childrenIt = objectTree.find("children");

        if(childrenIt != objectTree.not_found())
        {
            for(const auto& childIt : childrenIt->second)
            {
                children[childIt.first] = deepDeserialize(
                    cache,
                    archive,
                    childIt.second,
                    password
                );
            }
        }

        // Now, we can really deserialize the object
        const auto& newObject = deserializer->deserialize(archive, objectTree, children, object, password);

        if(newObject != object)
        {
            // This should not happen normally, only if the serializer doesn't reuse object
            newObject->setUUID(uuid);
            cache[uuid] = newObject;
        }

        // Construct field map
        sight::data::Object::FieldMapType fields;

        const auto& fields_it = objectTree.find("fields");

        if(fields_it != objectTree.not_found())
        {
            for(const auto& field_it : fields_it->second)
            {
                fields[field_it.first] = deepDeserialize(
                    cache,
                    archive,
                    field_it.second,
                    password
                );
            }
        }

        // Assign the deserialized fields
        newObject->setFields(fields);

        return newObject;
    }
}

//------------------------------------------------------------------------------

sight::data::Object::sptr SessionDeserializer::deserialize(
    const std::filesystem::path& archive_path,
    const core::crypto::secure_string& password
) const
{
    // Initialize the object cache
    std::map<std::string, sight::data::Object::sptr> cache;

    // Create the archive that contain everything
    const auto& archive = zip::ArchiveReader::shared(archive_path);

    // Create the tree used to store everything and read the index.json from the archive
    boost::property_tree::ptree tree;
    {
        // istream must be closed after this, since archive could only open files one by one
        const auto istream = archive->openFile(this->getIndexFilePath(), password);
        boost::property_tree::read_json(*istream, tree);
    }

    SIGHT_THROW_IF(
        "Empty '" << this->getIndexFilePath() << "' from archive '" << archive_path << "'.",
        tree.empty()
    );

    return deepDeserialize(cache, archive, tree, password);
}

} // namespace detail

} // namespace sight::io::session

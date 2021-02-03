/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "fwAtomConversion/mapper/Landmarks.hpp"

#include "fwAtomConversion/convert.hpp"
#include "fwAtomConversion/exception/ConversionNotManaged.hpp"
#include "fwAtomConversion/mapper/registry/macros.hpp"

#include <core/tools/UUID.hpp>

#include <data/Landmarks.hpp>

#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <boost/algorithm/string.hpp>

namespace fwAtomConversion
{
namespace mapper
{

//-----------------------------------------------------------------------------

fwAtomConversionRegisterMacro( ::fwAtomConversion::mapper::Landmarks, data::Landmarks);

//-----------------------------------------------------------------------------

::fwAtoms::Object::sptr Landmarks::convert( data::Object::sptr object,
                                            DataVisitor::AtomCacheType& cache )
{
    const camp::Class& metaclass = ::camp::classByName( object->getClassname() );
    ::fwAtomConversion::DataVisitor visitor( object, cache );
    metaclass.visit(visitor);
    ::fwAtoms::Object::sptr atom = visitor.getAtomObject();

    data::Landmarks::csptr landmarks = data::Landmarks::dynamicCast(object);

    ::fwAtoms::Map::sptr map = ::fwAtoms::Map::New();

    data::Landmarks::GroupNameContainer names = landmarks->getGroupNames();
    for (const auto& name: names)
    {
        const data::Landmarks::LandmarksGroup& group = landmarks->getGroup(name);
        ::fwAtoms::Object::sptr atomGroup = ::fwAtoms::Object::New();
        atomGroup->setMetaInfo("ID_METAINFO", core::tools::UUID::generateUUID());

        const std::string colorStr = std::to_string(group.m_color[0]) + ";" +
                                     std::to_string(group.m_color[1]) + ";" +
                                     std::to_string(group.m_color[2]) + ";" +
                                     std::to_string(group.m_color[3]);
        atomGroup->setAttribute("color", ::fwAtoms::String::New(colorStr));
        atomGroup->setAttribute("size", ::fwAtoms::Numeric::New(group.m_size));
        const std::string shapeStr = (group.m_shape == data::Landmarks::Shape::SPHERE) ? "SPHERE" : "CUBE";
        atomGroup->setAttribute("shape", ::fwAtoms::String::New(shapeStr));
        atomGroup->setAttribute("visibility", ::fwAtoms::Boolean::New(group.m_visibility));

        ::fwAtoms::Sequence::sptr seq = ::fwAtoms::Sequence::New();

        for (const auto& point : group.m_points)
        {
            const std::string pointStr = std::to_string(point[0]) + ";" +
                                         std::to_string(point[1]) + ";" +
                                         std::to_string(point[2]);
            seq->push_back(::fwAtoms::String::New(pointStr));
        }
        atomGroup->setAttribute("points", seq);
        map->insert(name, atomGroup);
    }
    atom->setAttribute("landmarks", map );

    return atom;
}

//-----------------------------------------------------------------------------

data::Object::sptr Landmarks::convert(  ::fwAtoms::Object::sptr atom,
                                        AtomVisitor::DataCacheType& cache,
                                        const AtomVisitor::IReadPolicy& uuidPolicy
                                        )
{
    ::fwAtomConversion::AtomVisitor visitor( atom, cache, uuidPolicy );
    visitor.visit();
    data::Object::sptr data         = visitor.getDataObject();
    data::Landmarks::sptr landmarks = data::Landmarks::dynamicCast(data);

    ::fwAtoms::Map::sptr map = ::fwAtoms::Map::dynamicCast(atom->getAttribute("landmarks"));

    for (const auto& elt : map->getValue())
    {
        const std::string name = elt.first;
        FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                   "sub atoms stored in fwAtom::Map 'landmarks' must be atom objects"),
                               elt.second->type() != ::fwAtoms::Base::OBJECT );
        ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::dynamicCast(elt.second);

        // get color
        ::fwAtoms::String::csptr colorObj = ::fwAtoms::String::dynamicCast(obj->getAttribute("color"));
        FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                   "sub atom 'color' stored in fwAtom::Object 'landmarks' must be ::fwAtoms::String"),
                               !colorObj );

        const std::string& colorStr = colorObj->getValue();

        std::vector< std::string> result;
        ::boost::split(result, colorStr, ::boost::is_any_of(";"));

        FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged("'color' atom must be of type rgba"),
                               result.size() != 4 );
        const data::Landmarks::ColorType color = {{
                                                      std::stof(result[0]), std::stof(result[1]),
                                                      std::stof(result[2]), std::stof(result[3])
                                                  }};

        // get size
        ::fwAtoms::Numeric::csptr sizeObj = ::fwAtoms::Numeric::dynamicCast(obj->getAttribute("size"));
        FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                   "sub atom 'size' stored in fwAtom::Object 'landmarks' must be ::fwAtoms::Numeric"),
                               !sizeObj );
        const data::Landmarks::SizeType size = sizeObj->getValue< data::Landmarks::SizeType >();

        // get shape
        ::fwAtoms::String::csptr shapeObj = ::fwAtoms::String::dynamicCast(obj->getAttribute("shape"));
        FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                   "sub atom 'shape' stored in fwAtom::Object 'landmarks' must be ::fwAtoms::String"),
                               !shapeObj );

        const std::string& shapeStr = shapeObj->getValue();
        data::Landmarks::Shape shape;
        if (shapeStr == "SPHERE")
        {
            shape = data::Landmarks::Shape::SPHERE;
        }
        else if (shapeStr == "CUBE")
        {
            shape = data::Landmarks::Shape::CUBE;
        }
        else
        {
            FW_RAISE_EXCEPTION(exception::ConversionNotManaged("'shape' value '"+ shapeStr +"' is not managed"));
        }

        // get visibility
        ::fwAtoms::Boolean::csptr visuObj = ::fwAtoms::Boolean::dynamicCast(obj->getAttribute("visibility"));
        FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                   "sub atom 'visibility' stored in 'landmarks' must be ::fwAtoms::Boolean"),
                               !visuObj );
        const bool visibility = visuObj->getValue();

        landmarks->addGroup(name, color, size, shape, visibility);

        // get points
        ::fwAtoms::Sequence::csptr seq = ::fwAtoms::Sequence::dynamicCast(obj->getAttribute("points"));
        for (const auto& elt : seq->getValue())
        {
            FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                       "sub atoms stored in 'points' must be ::fwAtoms::String"),
                                   elt->type() != ::fwAtoms::Base::STRING );

            ::fwAtoms::String::csptr ptStrObj = ::fwAtoms::String::dynamicCast(elt);
            const std::string& ptStr = ptStrObj->getValue();

            std::vector< std::string> resultPt;
            ::boost::split(resultPt, ptStr, ::boost::is_any_of(";"));

            FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged("point atom must be of type x;y;z"),
                                   resultPt.size() != 3 );

            data::Landmarks::PointType pt = {{
                                                 std::stod(resultPt[0]), std::stod(resultPt[1]),
                                                 std::stod(resultPt[2])
                                             }};
            landmarks->addPoint(name, pt);
        }
    }

    return landmarks;
}

//-----------------------------------------------------------------------------

} //namespace mapper
} //namespace fwAtomConversion

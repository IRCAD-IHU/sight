/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fwDataIO/reader/MeshReader.hpp"

#include "fwDataIO/reader/registry/macros.hpp"

#include <data/location/SingleFile.hpp>
#include <data/Mesh.hpp>

#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi.hpp>

#include <fstream>
#include <iostream>

fwDataIOReaderRegisterMacro( ::fwDataIO::reader::MeshReader );

namespace fwDataIO
{

namespace reader
{

//------------------------------------------------------------------------------

struct cell_data_offset_generator {
    data::Mesh::CellId current;
    cell_data_offset_generator()
    {
        current = 0;
    }
    //------------------------------------------------------------------------------

    int operator()()
    {
        data::Mesh::CellId res = current;
        current += 3;
        return static_cast<int>(res);
    }
};

//------------------------------------------------------------------------------

template <typename Iterator>
bool parseTrian2(Iterator first, Iterator last, data::Mesh::sptr mesh)
{
    using boost::spirit::qi::ulong_long;
    using boost::spirit::qi::int_;
    using boost::spirit::qi::float_;
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::qi::_1;
    using boost::spirit::qi::_2;
    using boost::spirit::qi::_3;
    using boost::spirit::qi::_4;
    using boost::spirit::qi::_5;
    using boost::spirit::qi::_6;
    using boost::spirit::qi::repeat;
    using boost::spirit::ascii::space;
    using boost::phoenix::push_back;
    using boost::phoenix::ref;
    namespace phx = boost::phoenix;

    data::Mesh::Size nbPoints = 1;
    data::Mesh::Size nbCells  = 1;
    data::Mesh::Size count    = 0;

    // Starting from boost 1.65, the function could not be deduced
    auto reserveFn =
        phx::bind(static_cast<size_t (data::Mesh::*)(
                                  data::Mesh::Size, data::Mesh::Size, data::Mesh::CellType,
                                  data::Mesh::Attributes)>(
                      &data::Mesh::reserve), mesh, std::ref(nbPoints), std::ref(
                      nbCells), data::Mesh::CellType::TRIANGLE,
                  data::Mesh::Attributes::CELL_NORMALS);

    // initialize the mesh with 1 point, 1 cell and cell normals to be able to lock all the arrays
    mesh->reserve(nbPoints, nbCells, data::Mesh::CellType::TRIANGLE, data::Mesh::Attributes::CELL_NORMALS);

    const auto lock = mesh->lock();

    bool r = phrase_parse(first, last,

                          //  Begin grammar
                          (
                              ulong_long
                              [
                                  ref(nbPoints) = _1,
                                  reserveFn

                              ]

                              >> repeat(ref(nbPoints))
                              [
                                  (float_ >> float_ >> float_)
                                  [
                                      phx::bind(static_cast< data::Mesh::PointId (data::Mesh::*)(
                                                                 data::Mesh::PointValueType,
                                                                 data::Mesh::PointValueType,
                                                                 data::Mesh::PointValueType)>(
                                                    &data::Mesh::pushPoint), mesh, _1, _2, _3)
                                  ]
                              ]

                              >> ulong_long
                              [
                                  ref(nbCells) = _1,
                                  reserveFn
                              ]

                              >> repeat(ref(nbCells))
                              [
                                  (int_ >> int_ >> int_ >> float_ >> float_ >> float_)
                                  [
                                      phx::bind(static_cast< data::Mesh::CellId (data::Mesh::*)(
                                                                 data::Mesh::PointId,
                                                                 data::Mesh::PointId,
                                                                 data::Mesh::PointId)>( &data::Mesh::
                                                                                        pushCell),
                                                mesh, _1, _2, _3),
                                      phx::bind(static_cast< void (data::Mesh::*)(
                                                                 data::Mesh::CellId,
                                                                 data::Mesh::NormalValueType,
                                                                 data::Mesh::NormalValueType,
                                                                 data::Mesh::NormalValueType)>(
                                                    &data::Mesh::setCellNormal), mesh, ref(count), _4, _5, _6),
                                      ref(count)++
                                  ]
                              ]
                          ),
                          //  End grammar

                          space
                          );

    mesh->adjustAllocatedMemory();

    // Check if normals array is filled of -1. values
    const float normalBadValue = -1.f;

    auto begin = mesh->begin< data::iterator::CellIterator >();
    auto end   = mesh->end< data::iterator::CellIterator >();

    const auto nbBadValues = std::count_if( begin, end,
                                            [&normalBadValue](
                                                const data::iterator::CellIterator::value_type& val)
            {
                return val.normal->nx == normalBadValue && val.normal->ny == normalBadValue && val.normal->nz == normalBadValue;
            });

    if (static_cast<size_t>(nbBadValues) == mesh->getNumberOfCells())
    {
        mesh->clearCellNormals();
        SLM_WARN("normals equals to (-1,-1,-1) : normals removed.");
    }

    if (first != last) // fail if we didn't get a full match
    {
        return false;
    }
    return r;

}

//------------------------------------------------------------------------------

MeshReader::MeshReader(::fwDataIO::reader::IObjectReader::Key) :
    data::location::enableSingleFile< IObjectReader >(this)
{
}

//------------------------------------------------------------------------------

MeshReader::~MeshReader()
{
}

//------------------------------------------------------------------------------

void MeshReader::read()
{
    assert( std::dynamic_pointer_cast< data::location::SingleFile >(m_location) );
    std::filesystem::path path = std::dynamic_pointer_cast< data::location::SingleFile >(m_location)->getPath();

    SLM_ASSERT("Empty path for Trian file", !path.empty() );

    std::string buf;
    std::ifstream file;
    file.open(path.string().c_str(), std::ios::binary );

    if (!file.is_open())
    {
        SLM_ERROR( "Trian file loading error for " + path.string());
        throw std::ios_base::failure("Unable to open " + path.string());
    }

    file.seekg(0, std::ios::end);
    const auto length = file.tellg();
    file.seekg(0, std::ios::beg);

    //buffer = new char [length];
    buf.resize(static_cast<size_t>(length));
    char* buffer = &buf[0];

    file.read(buffer, static_cast<std::streamsize>(length));
    file.close();

    data::Mesh::sptr mesh = getConcreteObject();

    mesh->clear();

    if (!parseTrian2(buffer, buffer+static_cast<size_t>(length), mesh))
    {
        SLM_ERROR( "Bad file format : " << path.string());
        throw std::ios_base::failure("Unable to open " + path.string() + " : Bad file format.");
    }

}

//------------------------------------------------------------------------------

std::string MeshReader::extension()
{
    return (".trian");
}

//------------------------------------------------------------------------------

} // namespace reader
} // namespace fwDataIO

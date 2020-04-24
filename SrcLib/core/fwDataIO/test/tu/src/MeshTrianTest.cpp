/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "MeshTrianTest.hpp"

#include <fwDataIO/reader/MeshReader.hpp>
#include <fwDataIO/writer/MeshWriter.hpp>

#include <fwDataTools/Mesh.hpp>

#include <fwTest/generator/Mesh.hpp>

#include <fwTools/System.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataIO::ut::MeshTrianTest );

namespace fwDataIO
{
namespace ut
{

//------------------------------------------------------------------------------

void MeshTrianTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void MeshTrianTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MeshTrianTest::testSimpleMesh()
{
    std::filesystem::path trianPath = ::fwTools::System::getTemporaryFolder() / "test.trian";

    ::fwData::Mesh::sptr mesh  = ::fwData::Mesh::New();
    ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    ::fwTest::generator::Mesh::generateTriangleMesh(mesh);
    ::fwDataTools::Mesh::shakePoint(mesh);

    mesh->adjustAllocatedMemory();

    ::fwDataIO::writer::MeshWriter::sptr writer = ::fwDataIO::writer::MeshWriter::New();
    writer->setObject(mesh);
    writer->setFile(trianPath);
    writer->write();

    ::fwDataIO::reader::MeshReader::sptr reader = ::fwDataIO::reader::MeshReader::New();

    reader->setObject(mesh2);
    reader->setFile(trianPath);
    reader->read();

    bool suppr = std::filesystem::remove(trianPath);
    CPPUNIT_ASSERT(suppr);

    this->compareMesh(mesh, mesh2);
}

//------------------------------------------------------------------------------

void MeshTrianTest::testMeshWithCellNormals()
{
    std::filesystem::path trianPath = ::fwTools::System::getTemporaryFolder() / "test.trian";

    ::fwData::Mesh::sptr mesh  = ::fwData::Mesh::New();
    ::fwData::Mesh::sptr mesh2 = ::fwData::Mesh::New();
    ::fwTest::generator::Mesh::generateTriangleMesh(mesh);
    ::fwDataTools::Mesh::shakePoint(mesh);
    ::fwDataTools::Mesh::generateCellNormals(mesh);

    mesh->adjustAllocatedMemory();

    ::fwDataIO::writer::MeshWriter::sptr writer = ::fwDataIO::writer::MeshWriter::New();
    writer->setObject(mesh);
    writer->setFile(trianPath);
    writer->write();

    ::fwDataIO::reader::MeshReader::sptr reader = ::fwDataIO::reader::MeshReader::New();

    reader->setObject(mesh2);
    reader->setFile(trianPath);
    reader->read();

    bool suppr = std::filesystem::remove(trianPath);
    CPPUNIT_ASSERT(suppr);

    this->compareMesh(mesh, mesh2);
}

//------------------------------------------------------------------------------

#define COMPAREBUFFER(type, buff1, buff2)                             \
    {                                                                     \
        CPPUNIT_ASSERT( (!buff1 && !buff2) || (buff1 && buff2));          \
        if(buff1)                                                         \
        {                                                                 \
            CPPUNIT_ASSERT(buff1->getSize() == buff2->getSize());         \
            ::fwDataTools::helper::Array helper1(buff1);                      \
            ::fwDataTools::helper::Array helper2(buff2);                      \
                                                                      \
            type* iter1 = helper1.begin<type>();                          \
            type* iter2 = helper2.begin<type>();                          \
                                                                      \
            for (; iter1 != helper1.end<type>(); ++iter1, ++iter2)       \
            {                                                             \
                CPPUNIT_ASSERT_DOUBLES_EQUAL(*iter1, *iter2, 0.000001);   \
            }                                                             \
        }                                                                 \
    }

//------------------------------------------------------------------------------

void MeshTrianTest::compareMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2)
{
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), mesh2->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells(), mesh2->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(mesh1->getCellDataSize(), mesh2->getCellDataSize());

    COMPAREBUFFER(::fwData::Mesh::PointValueType, mesh1->getPointsArray(), mesh2->getPointsArray());
    COMPAREBUFFER(::fwData::Mesh::CellTypes, mesh1->getCellTypesArray(), mesh2->getCellTypesArray());
    COMPAREBUFFER(::fwData::Mesh::CellDataOffsetType, mesh1->getCellDataOffsetsArray(),
                  mesh2->getCellDataOffsetsArray());
    COMPAREBUFFER(::fwData::Mesh::CellValueType, mesh1->getCellDataArray(), mesh2->getCellDataArray());
    COMPAREBUFFER(::fwData::Mesh::ColorValueType, mesh1->getPointColorsArray(), mesh2->getPointColorsArray());
    COMPAREBUFFER(::fwData::Mesh::ColorValueType, mesh1->getCellColorsArray(), mesh2->getCellColorsArray());
    COMPAREBUFFER(::fwData::Mesh::NormalValueType, mesh1->getPointNormalsArray(), mesh2->getPointNormalsArray());
    COMPAREBUFFER(::fwData::Mesh::NormalValueType, mesh1->getCellNormalsArray(), mesh2->getCellNormalsArray());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataIO

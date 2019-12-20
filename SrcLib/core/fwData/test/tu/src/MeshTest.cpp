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

#include "MeshTest.hpp"

#include <fwData/Mesh.hpp>
#include <fwData/ObjectLock.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/Mesh.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::MeshTest );

namespace fwData
{
namespace ut
{

//------------------------------------------------------------------------------

void MeshTest::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void MeshTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void MeshTest::deprecatedAllocation()
{
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    size_t pointSize    = 3000;
    size_t cellSize     = 2000;
    size_t cellDataSize = 8000;
    mesh->allocate(pointSize, cellSize, cellDataSize);
    CPPUNIT_ASSERT_EQUAL(pointSize, mesh->getPointsArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(cellSize, mesh->getCellTypesArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(cellDataSize, mesh->getCellDataArray()->getSize()[0]);

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(4), mesh->getPointsArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), mesh->getPointsArray()->getNumberOfComponents());
    size_t pointAllocatedSize = pointSize * mesh->getPointsArray()->getNumberOfComponents() *
                                mesh->getPointsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(pointAllocatedSize, mesh->getPointsArray()->getSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(1), mesh->getCellTypesArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), mesh->getCellTypesArray()->getNumberOfComponents());
    size_t cellTypeAllocatedSize = cellSize * mesh->getCellTypesArray()->getNumberOfComponents() *
                                   mesh->getCellTypesArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(cellTypeAllocatedSize, mesh->getCellTypesArray()->getSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(8), mesh->getCellDataOffsetsArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), mesh->getCellDataOffsetsArray()->getNumberOfComponents());
    size_t cellDataOffsetsAllocatedSize = cellSize * mesh->getCellDataOffsetsArray()->getNumberOfComponents() *
                                          mesh->getCellDataOffsetsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(cellDataOffsetsAllocatedSize, mesh->getCellDataOffsetsArray()->getSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(8), mesh->getCellDataArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), mesh->getCellDataArray()->getNumberOfComponents());
    size_t cellDataAllocatedSize = cellDataSize * mesh->getCellDataArray()->getNumberOfComponents() *
                                   mesh->getCellDataArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(cellDataAllocatedSize, mesh->getCellDataArray()->getSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(pointAllocatedSize + cellSize + cellDataOffsetsAllocatedSize + cellDataAllocatedSize,
                         mesh->getAllocatedSizeInBytes());

    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getNumberOfCells());

    ::fwDataTools::helper::Mesh helper(mesh);
    helper.insertNextPoint(10, 20, 30);
    helper.insertNextPoint(10, 10, 10);
    helper.insertNextPoint(20, 20, 10);

    helper.insertNextCell(1, 2, 3);
    CPPUNIT_ASSERT_EQUAL((fwData::Mesh::Id)3, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL((fwData::Mesh::Id)1, mesh->getNumberOfCells());

    mesh->allocatePointNormals();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfPoints()), mesh->getPointNormalsArray()->getSize()[0]);
    mesh->allocatePointColors(::fwData::Mesh::RGB);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfPoints()), mesh->getPointColorsArray()->getSize()[0]);
    mesh->allocatePointTexCoords();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfPoints()), mesh->getPointTexCoordsArray()->getSize()[0]);

    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(4), mesh->getPointNormalsArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), mesh->getPointNormalsArray()->getNumberOfComponents());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(1), mesh->getPointColorsArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), mesh->getPointColorsArray()->getNumberOfComponents());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(4), mesh->getPointTexCoordsArray()->getType().sizeOf());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), mesh->getPointTexCoordsArray()->getNumberOfComponents());

    size_t pointNormalsAllocatedSize = mesh->getNumberOfPoints() *
                                       mesh->getPointNormalsArray()->getNumberOfComponents() *
                                       mesh->getPointNormalsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(pointNormalsAllocatedSize, mesh->getPointNormalsArray()->getSizeInBytes());
    size_t pointColorsAllocatedSize = mesh->getNumberOfPoints() * mesh->getPointColorsArray()->getNumberOfComponents() *
                                      mesh->getPointColorsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(pointColorsAllocatedSize, mesh->getPointColorsArray()->getSizeInBytes());
    size_t pointTexCoordsAllocatedSize = mesh->getNumberOfPoints() *
                                         mesh->getPointTexCoordsArray()->getNumberOfComponents() *
                                         mesh->getPointTexCoordsArray()->getType().sizeOf();
    CPPUNIT_ASSERT_EQUAL(pointTexCoordsAllocatedSize, mesh->getPointTexCoordsArray()->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(
        pointAllocatedSize + cellSize + cellDataOffsetsAllocatedSize + cellDataAllocatedSize + pointNormalsAllocatedSize + pointColorsAllocatedSize + pointTexCoordsAllocatedSize,
        mesh->getAllocatedSizeInBytes());

    mesh->allocateCellNormals();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfCells()), mesh->getCellNormalsArray()->getSize()[0]);
    mesh->allocateCellColors(::fwData::Mesh::RGBA);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfCells()), mesh->getCellColorsArray()->getSize()[0]);
    mesh->allocateCellTexCoords();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfCells()), mesh->getCellTexCoordsArray()->getSize()[0]);

    size_t oldsize = mesh->getAllocatedSizeInBytes();
    bool adjusted  = mesh->adjustAllocatedMemory();
    size_t newSize = mesh->getAllocatedSizeInBytes();
    CPPUNIT_ASSERT_EQUAL(true, adjusted);
    CPPUNIT_ASSERT(oldsize > newSize);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfPoints()), mesh->getPointsArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getNumberOfCells()), mesh->getCellTypesArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(mesh->getCellDataSize()), mesh->getCellDataArray()->getSize()[0]);

    mesh->clear();
    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id) 0, mesh->getCellDataSize());
    CPPUNIT_ASSERT_EQUAL((size_t) 0, mesh->getDataSizeInBytes());

    ::fwData::Mesh::Id nbPoints        = 300;
    ::fwData::Mesh::Id nbCells         = 200;
    ::fwData::Mesh::Id newCellDataSize = 500;
    mesh->setNumberOfPoints(nbPoints);
    mesh->setNumberOfCells(nbCells);
    mesh->setCellDataSize(newCellDataSize);
    oldsize  = mesh->getAllocatedSizeInBytes();
    adjusted = mesh->adjustAllocatedMemory();
    newSize  = mesh->getAllocatedSizeInBytes();
    CPPUNIT_ASSERT_EQUAL(true, adjusted);
    CPPUNIT_ASSERT(oldsize < newSize);

    CPPUNIT_ASSERT_EQUAL(nbPoints, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(nbCells, mesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(newCellDataSize, mesh->getCellDataSize());

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(nbPoints), mesh->getPointsArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(nbCells), mesh->getCellTypesArray()->getSize()[0]);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(newCellDataSize), mesh->getCellDataArray()->getSize()[0]);
}

//-----------------------------------------------------------------------------

void MeshTest::deprecatedInsertion()
{
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    ::fwDataTools::helper::Mesh helper(mesh);
    helper.insertNextPoint(10, 20, 30);
    helper.insertNextPoint(40, 40, 60);
    helper.insertNextPoint(70, 80, 90);
    helper.insertNextPoint(15, 25, 35);
    helper.insertNextPoint(45, 55, 65);
    helper.insertNextPoint(12, 22, 32);
    helper.insertNextPoint(42, 52, 62);
    helper.insertNextPoint(100, 200, 300);

    helper.insertNextCell(1, 2);
    helper.insertNextCell(1, 3, 4);
    helper.insertNextCell(1, 2, 5, 4);

    ::fwData::Mesh::CellValueType p[4] = {3, 6, 5, 1};
    helper.insertNextCell(::fwData::Mesh::QUAD, p, 4);

    ::fwData::Mesh::CellValueType p2[6] = {1, 3, 5, 7, 2, 6};
    helper.insertNextCell(::fwData::Mesh::POLY, p2, 6);

    ::fwData::Mesh::CellValueType p3[5] = {7, 2, 5, 4, 3};
    helper.insertNextCell(::fwData::Mesh::POLY, p3, 5);

    CPPUNIT_ASSERT_EQUAL((fwData::Mesh::Id)8, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL((fwData::Mesh::Id)6, mesh->getNumberOfCells());

    ::fwData::Mesh::PointsMultiArrayType pointArray = helper.getPoints();
    CPPUNIT_ASSERT_EQUAL((float) 10, pointArray[0][0]);
    CPPUNIT_ASSERT_EQUAL((float) 70, pointArray[2][0]);
    CPPUNIT_ASSERT_EQUAL((float) 80, pointArray[2][1]);
    CPPUNIT_ASSERT_EQUAL((float) 300, pointArray[7][2]);

    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsetArray = helper.getCellDataOffsets();
    CPPUNIT_ASSERT_EQUAL( (std::uint64_t)0, cellDataOffsetArray[0]);
    CPPUNIT_ASSERT_EQUAL( (std::uint64_t)2, cellDataOffsetArray[1]);
    CPPUNIT_ASSERT_EQUAL( (std::uint64_t)9, cellDataOffsetArray[3]);
    CPPUNIT_ASSERT_EQUAL( (std::uint64_t)13, cellDataOffsetArray[4]);
    CPPUNIT_ASSERT_EQUAL( (std::uint64_t)19, cellDataOffsetArray[5]);

    ::fwData::Mesh::CellDataMultiArrayType cellDataArray = helper.getCellData();
    CPPUNIT_ASSERT_EQUAL( (std::uint64_t)1,
                          cellDataArray[static_cast< ::fwData::Mesh::CellDataMultiArrayType::index >(
                                            cellDataOffsetArray[0])]);
    CPPUNIT_ASSERT_EQUAL( (std::uint64_t)1,
                          cellDataArray[static_cast< ::fwData::Mesh::CellDataMultiArrayType::index >(
                                            cellDataOffsetArray[1])]);
    CPPUNIT_ASSERT_EQUAL( (std::uint64_t)3,
                          cellDataArray[static_cast< ::fwData::Mesh::CellDataMultiArrayType::index >(
                                            cellDataOffsetArray[3])]);
    CPPUNIT_ASSERT_EQUAL( (std::uint64_t)1,
                          cellDataArray[static_cast< ::fwData::Mesh::CellDataMultiArrayType::index >(
                                            cellDataOffsetArray[4])]);
    CPPUNIT_ASSERT_EQUAL( (std::uint64_t)7,
                          cellDataArray[static_cast< ::fwData::Mesh::CellDataMultiArrayType::index >(
                                            cellDataOffsetArray[5])]);

    ::fwData::Mesh::CellTypesMultiArrayType cellTypesArray = helper.getCellTypes();
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::EDGE, static_cast< ::fwData::Mesh::CellTypesEnum >(cellTypesArray[0]));
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::TRIANGLE, static_cast < ::fwData::Mesh::CellTypesEnum >(cellTypesArray[1]));
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::QUAD, static_cast< ::fwData::Mesh::CellTypesEnum >(cellTypesArray[3]));
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::POLY, static_cast< ::fwData::Mesh::CellTypesEnum >(cellTypesArray[4]));
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::POLY, static_cast< ::fwData::Mesh::CellTypesEnum >(cellTypesArray[5]));

    helper.setPoint(4, 45, 59, 45);
    CPPUNIT_ASSERT_EQUAL((float) 45, pointArray[4][0]);
    CPPUNIT_ASSERT_EQUAL((float) 59, pointArray[4][1]);
    CPPUNIT_ASSERT_EQUAL((float) 45, pointArray[4][2]);

    ::fwData::Array::sptr array = mesh->getPointsArray();
    ::fwDataTools::helper::Array arrayHelper(array);
    CPPUNIT_ASSERT_EQUAL(*(arrayHelper.getItem< ::fwData::Mesh::PointValueType >({4}, 0)), pointArray[4][0]);
    CPPUNIT_ASSERT_EQUAL(*(arrayHelper.getItem< ::fwData::Mesh::PointValueType >({4}, 1)), pointArray[4][1]);
    CPPUNIT_ASSERT_EQUAL(*(arrayHelper.getItem< ::fwData::Mesh::PointValueType >({4}, 2)), pointArray[4][2]);
}

//-----------------------------------------------------------------------------

void MeshTest::colorsNormals()
{
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    ::fwDataTools::helper::Mesh helper(mesh);
    helper.insertNextPoint(10, 20, 30);
    helper.insertNextPoint(10, 10, 10);
    helper.insertNextPoint(20, 20, 10);
    helper.insertNextPoint(30, 30, 10);
    helper.insertNextPoint(15, 20, 35);
    helper.insertNextPoint(20, 20, 10);
    helper.insertNextPoint(20, 63, 17);
    helper.insertNextPoint(27, 83, 52);

    helper.insertNextCell(1, 2);
    helper.insertNextCell(1, 3, 4);
    helper.insertNextCell(1, 2, 5, 4);

    ::fwData::Mesh::CellValueType p[4] = {3, 6, 5, 1};
    helper.insertNextCell(::fwData::Mesh::QUAD, p, 4);

    ::fwData::Mesh::CellValueType p2[6] = {1, 3, 5, 7, 2, 6};
    helper.insertNextCell(::fwData::Mesh::POLY, p2, 6);

    ::fwData::Mesh::CellValueType p3[5] = {7, 2, 5, 4, 3};
    helper.insertNextCell(::fwData::Mesh::POLY, p3, 5);

    mesh->allocatePointColors(::fwData::Mesh::RGBA);
    helper.updateLock();
    ::fwData::Mesh::PointColorsMultiArrayType pointColorArray = helper.getPointColors();

    const long nbPoints = static_cast<long>(mesh->getNumberOfPoints());
    for (long id = 0; id < nbPoints; id++)
    {
        for (long c = 0; c < 4; c++)
        {
            pointColorArray[id][c] = static_cast< ::fwData::Mesh::ColorValueType >(id * 10 + c);
        }
    }
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::ColorValueType >( 0), pointColorArray[0][0]);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::ColorValueType >(21), pointColorArray[2][1]);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::ColorValueType >(50), pointColorArray[5][0]);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::ColorValueType >(73), pointColorArray[7][3]);

    ::fwData::Mesh::ColorValueType color[4] = {5, 8, 6, 10};
    helper.setPointColor(5, color);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::ColorValueType >(8), pointColorArray[5][1]);

    mesh->allocateCellNormals();
    helper.updateLock();
    ::fwData::Mesh::CellNormalsMultiArrayType cellNormalArray = helper.getCellNormals();

    const long nbCells = static_cast<long>(mesh->getNumberOfCells());
    for (long id = 0; id < nbCells; id++)
    {
        for (long n = 0; n < 3; n++)
        {
            cellNormalArray[id][n] = static_cast<float>(id) + static_cast<float>(n)/10.f;
        }
    }
    CPPUNIT_ASSERT_EQUAL(0.0f, cellNormalArray[0][0]);
    CPPUNIT_ASSERT_EQUAL(2.1f, cellNormalArray[2][1]);
    CPPUNIT_ASSERT_EQUAL(4.2f, cellNormalArray[4][2]);
    CPPUNIT_ASSERT_EQUAL(5.0f, cellNormalArray[5][0]);

    ::fwData::Mesh::NormalValueType normal[3] = {0.9f, 0.4f, 0.2f};
    helper.setCellNormal(4, normal);
    CPPUNIT_ASSERT_EQUAL(0.2f, cellNormalArray[4][2]);
}

//-----------------------------------------------------------------------------

void MeshTest::texCoords()
{
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    ::fwDataTools::helper::Mesh helper(mesh);
    helper.insertNextPoint(10, 20, 30);
    helper.insertNextPoint(10, 10, 10);
    helper.insertNextPoint(20, 20, 10);
    helper.insertNextPoint(30, 30, 10);
    helper.insertNextPoint(15, 20, 35);
    helper.insertNextPoint(20, 20, 10);
    helper.insertNextPoint(20, 63, 17);
    helper.insertNextPoint(27, 83, 52);

    helper.insertNextCell(1, 2);
    helper.insertNextCell(1, 3, 4);
    helper.insertNextCell(1, 2, 5, 4);

    ::fwData::Mesh::CellValueType p[4] = {3, 6, 5, 1};
    helper.insertNextCell(::fwData::Mesh::QUAD, p, 4);

    ::fwData::Mesh::CellValueType p2[6] = {1, 3, 5, 7, 2, 6};
    helper.insertNextCell(::fwData::Mesh::POLY, p2, 6);

    ::fwData::Mesh::CellValueType p3[5] = {7, 2, 5, 4, 3};
    helper.insertNextCell(::fwData::Mesh::POLY, p3, 5);

    mesh->allocateCellTexCoords();
    helper.updateLock();
    ::fwData::Mesh::CellTexCoordsMultiArrayType cellTexCoordArray = helper.getCellTexCoords();

    const long nbCells = static_cast<long>(mesh->getNumberOfCells());
    for (long id = 0; id < nbCells; id++)
    {
        for (long n = 0; n < 2; n++)
        {
            cellTexCoordArray[id][n] = static_cast<float>(id) + static_cast<float>(n)/10.f;
        }
    }
    CPPUNIT_ASSERT_EQUAL(0.0f, cellTexCoordArray[0][0]);
    CPPUNIT_ASSERT_EQUAL(2.1f, cellTexCoordArray[2][1]);
    CPPUNIT_ASSERT_EQUAL(4.1f, cellTexCoordArray[4][1]);
    CPPUNIT_ASSERT_EQUAL(5.0f, cellTexCoordArray[5][0]);

    ::fwData::Mesh::TexCoordValueType TexCoord[3] = {0.9f, 0.4f};
    helper.setCellTexCoord(4, TexCoord);
    CPPUNIT_ASSERT_EQUAL(0.9f, cellTexCoordArray[4][0]);
    CPPUNIT_ASSERT_EQUAL(0.4f, cellTexCoordArray[4][1]);
}

//-----------------------------------------------------------------------------

void MeshTest::addingArray()
{
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

    ::fwData::Mesh::Id nbPoints = 60;
    ::fwData::Mesh::Id nbCells  = 20;

    // Add point array
    ::fwData::Array::sptr pointArray = ::fwData::Array::New();

    pointArray->resize(::fwTools::Type::create< ::fwData::Mesh::PointValueType >(), {nbPoints}, 3, true);
    ::fwDataTools::helper::Array pointArrayHelper(pointArray);

    ::fwData::Mesh::PointValueType count = 0;
    ::fwData::Mesh::PointValueType* iter = pointArrayHelper.begin< ::fwData::Mesh::PointValueType >();
    for (; iter != pointArrayHelper.end< ::fwData::Mesh::PointValueType >(); ++iter)
    {
        for (int i = 0; i < 3; i++)
        {
            *iter = count + static_cast<float>(i)/10.0f;
        }
        count++;
    }

    mesh->setPointsArray(pointArray);
    mesh->setNumberOfPoints(nbPoints);

    CPPUNIT_ASSERT_EQUAL(nbPoints, mesh->getNumberOfPoints());

    ::fwDataTools::helper::Mesh meshHelper(mesh);
    ::fwData::Mesh::Id id = meshHelper.insertNextPoint(50, 50, 50);
    CPPUNIT_ASSERT_EQUAL(nbPoints, id);
    nbPoints++;
    CPPUNIT_ASSERT_EQUAL(nbPoints, mesh->getNumberOfPoints());

    ::fwData::Mesh::PointsMultiArrayType pointMultiArray = meshHelper.getPoints();
    CPPUNIT_ASSERT_EQUAL(*(pointArrayHelper.getItem< ::fwData::Mesh::PointValueType >({0}, 0)),
                         pointMultiArray[0][0]);
    CPPUNIT_ASSERT_EQUAL(*(pointArrayHelper.getItem< ::fwData::Mesh::PointValueType >({12}, 2)),
                         pointMultiArray[12][2]);
    CPPUNIT_ASSERT_EQUAL(*(pointArrayHelper.getItem< ::fwData::Mesh::PointValueType >({45}, 1)),
                         pointMultiArray[45][1]);

    // add cells arrays
    ::fwData::Array::sptr cellTypeArray       = ::fwData::Array::New();
    ::fwData::Array::sptr cellDataOffsetArray = ::fwData::Array::New();
    ::fwData::Array::sptr cellDataArray       = ::fwData::Array::New();

    cellTypeArray->resize(::fwTools::Type::create< ::fwData::Mesh::CellTypes >(), {nbCells}, 1, true);
    cellDataOffsetArray->resize(::fwTools::Type::create< ::fwData::Mesh::CellDataOffsetType >(), {nbCells}, 1, true);
    cellDataArray->resize(::fwTools::Type::create< ::fwData::Mesh::CellValueType >(), {nbCells*3}, 1, true);

    ::fwDataTools::helper::Array cellTypeArrayHelper(cellTypeArray);
    ::fwDataTools::helper::Array cellDataOffsetArrayHelper(cellDataOffsetArray);
    ::fwDataTools::helper::Array cellDataArrayHelper(cellDataArray);

    ::fwData::Mesh::CellValueType counter = 0;
    for (std::uint64_t idCell = 0; idCell < nbCells; idCell++)
    {
        ::fwData::Mesh::CellTypes type = static_cast< ::fwData::Mesh::CellTypes >(::fwData::Mesh::TRIANGLE);
        cellTypeArrayHelper.setItem({size_t(idCell)}, &type);

        ::fwData::Mesh::CellDataOffsetType offset = idCell*3;
        cellDataOffsetArrayHelper.setItem({size_t(idCell)}, &offset);

        ::fwData::Mesh::CellValueType cell[3] = { counter, counter+1, counter+2 };
        counter                              += 3;
        cellDataArrayHelper.setItem({offset}, &cell);
    }
    mesh->setCellTypesArray(cellTypeArray);
    mesh->setCellDataOffsetsArray(cellDataOffsetArray);
    mesh->setCellDataArray(cellDataArray);
    mesh->setNumberOfCells(nbCells);
    mesh->setCellDataSize(nbCells*3);

    meshHelper.updateLock();
    meshHelper.insertNextCell(1, 6, 25, 12 );

    nbCells++;
    CPPUNIT_ASSERT_EQUAL(nbCells, mesh->getNumberOfCells());

    ::fwData::Mesh::CellTypesMultiArrayType cellTypesMultiArray = meshHelper.getCellTypes();
    CPPUNIT_ASSERT_EQUAL(*(cellTypeArrayHelper.getItem< ::fwData::Mesh::CellTypes >({0})),
                         cellTypesMultiArray[0]);
    CPPUNIT_ASSERT_EQUAL(*(cellTypeArrayHelper.getItem< ::fwData::Mesh::CellTypes >({12})),
                         cellTypesMultiArray[12]);
    CPPUNIT_ASSERT_EQUAL(*(cellTypeArrayHelper.getItem< ::fwData::Mesh::CellTypes >({18})),
                         cellTypesMultiArray[18]);

    ::fwData::Mesh::CellDataOffsetsMultiArrayType cellDataOffsetsMultiArray = meshHelper.getCellDataOffsets();
    CPPUNIT_ASSERT_EQUAL(*(cellDataOffsetArrayHelper.getItem< ::fwData::Mesh::CellDataOffsetType >({0})),
                         cellDataOffsetsMultiArray[0]);
    CPPUNIT_ASSERT_EQUAL(*(cellDataOffsetArrayHelper.getItem< ::fwData::Mesh::CellDataOffsetType >({12})),
                         cellDataOffsetsMultiArray[12]);
    CPPUNIT_ASSERT_EQUAL(*(cellDataOffsetArrayHelper.getItem< ::fwData::Mesh::CellDataOffsetType >({18})),
                         cellDataOffsetsMultiArray[18]);

    ::fwData::Mesh::CellDataOffsetType offset0  = cellTypesMultiArray[0];
    ::fwData::Mesh::CellDataOffsetType offset12 = cellTypesMultiArray[12];
    ::fwData::Mesh::CellDataOffsetType offset18 = cellTypesMultiArray[18];

    ::fwData::Mesh::CellDataMultiArrayType cellDataMultiArray = meshHelper.getCellData();
    CPPUNIT_ASSERT_EQUAL(*(cellDataArrayHelper.getItem< ::fwData::Mesh::CellValueType >({offset0})),
                         cellDataMultiArray[static_cast< ::fwData::Mesh::CellDataMultiArrayType::index >(offset0)]);
    CPPUNIT_ASSERT_EQUAL(*(cellDataArrayHelper.getItem< ::fwData::Mesh::CellValueType >({offset12})),
                         cellDataMultiArray[static_cast< ::fwData::Mesh::CellDataMultiArrayType::index >(offset12)]);
    CPPUNIT_ASSERT_EQUAL(*(cellDataArrayHelper.getItem< ::fwData::Mesh::CellValueType >({offset18})),
                         cellDataMultiArray[static_cast< ::fwData::Mesh::CellDataMultiArrayType::index >(offset18)]);
}

//-----------------------------------------------------------------------------

void MeshTest::copy()
{
    ::fwData::Mesh::sptr mesh            = ::fwData::Mesh::New();
    ::fwData::Mesh::sptr shallowCopyMesh = ::fwData::Mesh::New();

    ::fwDataTools::helper::Mesh meshHelper(mesh);

    meshHelper.insertNextPoint(10, 20, 30);
    meshHelper.insertNextPoint(10, 10, 10);
    meshHelper.insertNextPoint(20, 20, 10);
    meshHelper.insertNextPoint(30, 30, 10);
    meshHelper.insertNextPoint(15, 20, 35);
    meshHelper.insertNextPoint(20, 20, 10);
    meshHelper.insertNextPoint(20, 63, 17);
    meshHelper.insertNextPoint(27, 83, 52);

    meshHelper.insertNextCell(1, 2);
    meshHelper.insertNextCell(1, 3, 4);
    meshHelper.insertNextCell(1, 2, 5, 4);

    ::fwData::Mesh::CellValueType p[4] = {3, 6, 5, 1};
    meshHelper.insertNextCell(::fwData::Mesh::QUAD, p, 4);

    ::fwData::Mesh::CellValueType p2[6] = {1, 3, 5, 7, 2, 6};
    meshHelper.insertNextCell(::fwData::Mesh::POLY, p2, 6);

    ::fwData::Mesh::CellValueType p3[5] = {7, 2, 5, 4, 3};
    meshHelper.insertNextCell(::fwData::Mesh::POLY, p3, 5);

    mesh->allocatePointColors(::fwData::Mesh::RGBA);
    mesh->allocatePointTexCoords();
    meshHelper.updateLock();

    // check deep copy
    ::fwData::Mesh::sptr deepCopyMesh;
    deepCopyMesh = ::fwData::Object::copy(mesh);
    ::fwDataTools::helper::Mesh deepCopyMeshHelper(deepCopyMesh);

    CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfPoints(), deepCopyMesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfCells(), deepCopyMesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellDataSize(), deepCopyMesh->getCellDataSize());

    CPPUNIT_ASSERT(mesh->getPointsArray()->getSize() == deepCopyMesh->getPointsArray()->getSize());
    CPPUNIT_ASSERT(mesh->getCellTypesArray()->getSize() == deepCopyMesh->getCellTypesArray()->getSize());
    CPPUNIT_ASSERT(mesh->getCellDataOffsetsArray()->getSize() == deepCopyMesh->getCellDataOffsetsArray()->getSize());
    CPPUNIT_ASSERT(mesh->getCellDataArray()->getSize() == deepCopyMesh->getCellDataArray()->getSize());
    CPPUNIT_ASSERT(mesh->getPointColorsArray()->getSize() == deepCopyMesh->getPointColorsArray()->getSize());
    CPPUNIT_ASSERT(mesh->getPointTexCoordsArray()->getSize() == deepCopyMesh->getPointTexCoordsArray()->getSize());

    ::fwData::Mesh::PointsMultiArrayType meshPointArray         = meshHelper.getPoints();
    ::fwData::Mesh::PointsMultiArrayType deepCopyMeshPointArray = deepCopyMeshHelper.getPoints();

    const long nbPoints = static_cast<long>(mesh->getNumberOfPoints());
    for (long i = 0; i < nbPoints; i++)
    {
        for (long j = 0; j < 3; j++)
        {
            CPPUNIT_ASSERT_EQUAL(meshPointArray[i][j], deepCopyMeshPointArray[i][j]);
        }
    }

    //check shallow copy
    shallowCopyMesh->shallowCopy(mesh);
    CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfPoints(), shallowCopyMesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(mesh->getNumberOfCells(), shallowCopyMesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellDataSize(), shallowCopyMesh->getCellDataSize());

    CPPUNIT_ASSERT_EQUAL(mesh->getPointsArray(), shallowCopyMesh->getPointsArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellTypesArray(), shallowCopyMesh->getCellTypesArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellDataOffsetsArray(), shallowCopyMesh->getCellDataOffsetsArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellDataArray(), shallowCopyMesh->getCellDataArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getPointColorsArray(), shallowCopyMesh->getPointColorsArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getPointNormalsArray(), shallowCopyMesh->getPointNormalsArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getPointTexCoordsArray(), shallowCopyMesh->getPointTexCoordsArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellColorsArray(), shallowCopyMesh->getCellColorsArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellNormalsArray(), shallowCopyMesh->getCellNormalsArray());
    CPPUNIT_ASSERT_EQUAL(mesh->getCellTexCoordsArray(), shallowCopyMesh->getCellTexCoordsArray());
}

//------------------------------------------------------------------------------

void MeshTest::allocationTest()
{
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    size_t pointSize    = 3000;
    size_t cellSize     = 2000;
    size_t cellDataSize = 8000;

    const auto lock = mesh->lock();

    mesh->pushPoint(10, 20, 30);
    mesh->pushPoint(10, 10, 10);
    mesh->pushPoint(20, 20, 10);

    mesh->pushCell(0, 1, 2);
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id >(3), mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id >(1), mesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id >(3), mesh->getCellDataSize());

    size_t pointAllocatedSize           = mesh->getNumberOfPoints() * 3 * sizeof (::fwData::Mesh::PointValueType);
    size_t cellTypeAllocatedSize        = mesh->getNumberOfCells() * sizeof (::fwData::Mesh::CellTypes);
    size_t cellDataOffsetsAllocatedSize = mesh->getNumberOfCells() * sizeof (::fwData::Mesh::CellDataOffsetType);
    size_t cellDataAllocatedSize        = mesh->getCellDataSize() * sizeof(::fwData::Mesh::CellValueType);

    CPPUNIT_ASSERT_EQUAL(
        pointAllocatedSize + cellTypeAllocatedSize + cellDataOffsetsAllocatedSize + cellDataAllocatedSize,
        mesh->getDataSizeInBytes());

    mesh->reserve(pointSize, cellSize, cellDataSize);

    pointAllocatedSize           = pointSize * 3 * sizeof (::fwData::Mesh::PointValueType);
    cellTypeAllocatedSize        = cellSize * sizeof (::fwData::Mesh::CellTypes);
    cellDataOffsetsAllocatedSize = cellSize * sizeof (::fwData::Mesh::CellDataOffsetType);
    cellDataAllocatedSize        = cellDataSize * sizeof(::fwData::Mesh::CellValueType);

    CPPUNIT_ASSERT_EQUAL(
        pointAllocatedSize + cellTypeAllocatedSize + cellDataOffsetsAllocatedSize + cellDataAllocatedSize,
        mesh->getAllocatedSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id >(3), mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id >(1), mesh->getNumberOfCells());

    CPPUNIT_ASSERT_EQUAL(false, mesh->hasPointColors());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellColors());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasPointNormals());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellNormals());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasPointTexCoords());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellTexCoords());

    mesh->reserve(pointSize, cellSize, cellDataSize, ::fwData::Mesh::Attributes::POINT_NORMALS |
                  ::fwData::Mesh::Attributes::POINT_COLORS | ::fwData::Mesh::Attributes::POINT_TEX_COORDS);

    size_t pointNormalsAllocatedSize   = pointSize * 3 * sizeof (::fwData::Mesh::NormalValueType);
    size_t pointColorsAllocatedSize    = pointSize * 4 * sizeof (::fwData::Mesh::ColorValueType);
    size_t pointTexCoordsAllocatedSize = pointSize * 2 * sizeof (::fwData::Mesh::TexCoordValueType);
    CPPUNIT_ASSERT_EQUAL(
        pointAllocatedSize + cellTypeAllocatedSize + cellDataOffsetsAllocatedSize + cellDataAllocatedSize +
        pointNormalsAllocatedSize + pointColorsAllocatedSize + pointTexCoordsAllocatedSize,
        mesh->getAllocatedSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(true, mesh->hasPointColors());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellColors());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasPointNormals());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellNormals());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasPointTexCoords());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellTexCoords());

    mesh->reserve(pointSize, cellSize, cellDataSize,
                  ::fwData::Mesh::Attributes::POINT_NORMALS |
                  ::fwData::Mesh::Attributes::POINT_COLORS |
                  ::fwData::Mesh::Attributes::POINT_TEX_COORDS |
                  ::fwData::Mesh::Attributes::CELL_NORMALS |
                  ::fwData::Mesh::Attributes::CELL_COLORS |
                  ::fwData::Mesh::Attributes::CELL_TEX_COORDS);

    size_t oldsize = mesh->getAllocatedSizeInBytes();
    bool adjusted  = mesh->adjustAllocatedMemory();
    size_t newSize = mesh->getAllocatedSizeInBytes();

    CPPUNIT_ASSERT_EQUAL(true, adjusted);
    CPPUNIT_ASSERT(oldsize > newSize);

    CPPUNIT_ASSERT_EQUAL(true, mesh->hasPointColors());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasCellColors());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasPointNormals());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasCellNormals());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasPointTexCoords());
    CPPUNIT_ASSERT_EQUAL(true, mesh->hasCellTexCoords());

    mesh->clear();
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id>(0), mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id>(0), mesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id>(0), mesh->getCellDataSize());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), mesh->getDataSizeInBytes());

    CPPUNIT_ASSERT_EQUAL(false, mesh->hasPointColors());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellColors());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasPointNormals());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellNormals());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasPointTexCoords());
    CPPUNIT_ASSERT_EQUAL(false, mesh->hasCellTexCoords());

    ::fwData::Mesh::Id nbPoints        = 300;
    ::fwData::Mesh::Id nbCells         = 200;
    ::fwData::Mesh::Id newCellDataSize = 500;
    mesh->setNumberOfPoints(nbPoints);
    mesh->setNumberOfCells(nbCells);
    mesh->setCellDataSize(newCellDataSize);
    oldsize  = mesh->getAllocatedSizeInBytes();
    adjusted = mesh->adjustAllocatedMemory();
    newSize  = mesh->getAllocatedSizeInBytes();
    CPPUNIT_ASSERT_EQUAL(true, adjusted);
    CPPUNIT_ASSERT(oldsize < newSize);

    CPPUNIT_ASSERT_EQUAL(nbPoints, mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(nbCells, mesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(newCellDataSize, mesh->getCellDataSize());
}

//------------------------------------------------------------------------------

void MeshTest::insertion()
{
    {
        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
        const auto lock = mesh->lock();
        mesh->pushPoint(10, 20, 30);
        mesh->pushPoint(10, 10, 10);
        mesh->pushPoint(20, 21, 10);
        mesh->pushPoint(30, 30, 10);
        mesh->pushPoint(15, 20, 35);
        mesh->pushPoint(20, 20, 10);
        mesh->pushPoint(20, 63, 17);
        mesh->pushPoint(27, 83, 52);

        mesh->pushCell(1, 2);
        mesh->pushCell(1, 3, 4);
        mesh->pushCell(1, 2, 5, 4);

        ::fwData::Mesh::CellValueType p[4] = {3, 6, 5, 1};
        mesh->pushCell(::fwData::Mesh::CellType::QUAD, p, 4);

        ::fwData::Mesh::CellValueType p2[6] = {1, 3, 5, 7, 2, 6};
        mesh->pushCell(::fwData::Mesh::CellType::POLY, p2, 6);

        ::fwData::Mesh::CellValueType p3[5] = {7, 2, 5, 4, 3};
        mesh->pushCell(::fwData::Mesh::CellType::POLY, p3, 5);

        CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id>(8), mesh->getNumberOfPoints());
        CPPUNIT_ASSERT_EQUAL(static_cast< ::fwData::Mesh::Id>(6), mesh->getNumberOfCells());

        auto it = mesh->begin< ::fwData::iterator::PointIterator >();
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(10), (*it).point->x);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(20), (*it).point->y);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(30), (*it).point->z);
        CPPUNIT_ASSERT( nullptr == it->rgba);
        CPPUNIT_ASSERT( nullptr == it->normal);
        CPPUNIT_ASSERT( nullptr == it->tex);
        it += 2;
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(20), it->point->x);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(21), it->point->y);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(10), it->point->z);
        CPPUNIT_ASSERT( nullptr == it->rgba);
        CPPUNIT_ASSERT( nullptr == it->normal);
        CPPUNIT_ASSERT( nullptr == it->tex);
        it += 5;
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(27), it->point->x);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(83), it->point->y);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(52), it->point->z);
        --it;
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(20), it->point->x);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(63), it->point->y);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(17), it->point->z);
        it -= 2;
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(15), it->point->x);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(20), it->point->y);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(35), it->point->z);

        auto cellit = mesh->begin< ::fwData::iterator::CellIterator >();
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(0), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::EDGE, static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(2), cellit.nbPoints());
        CPPUNIT_ASSERT( nullptr == cellit->rgba);
        CPPUNIT_ASSERT( nullptr == cellit->normal);
        CPPUNIT_ASSERT( nullptr == cellit->tex);
        ++cellit;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(4), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(4), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::TRIANGLE,
                              static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(3), cellit.nbPoints());
        cellit += 2;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(9), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[3]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::QUAD, static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(4), cellit.nbPoints());
        cellit++;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(13), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(7), cellit->pointIdx[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit->pointIdx[4]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit->pointIdx[5]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(7), cellit[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit[4]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit[5]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::POLY, static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(6), cellit.nbPoints());
        cellit += 1;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(19), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(7), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(4), cellit->pointIdx[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[4]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(7), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(4), cellit[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[4]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::POLY, static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(5), cellit->nbPoints);
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(5), cellit.nbPoints());
        CPPUNIT_ASSERT( nullptr == cellit->rgba);
        CPPUNIT_ASSERT( nullptr == cellit->normal);
        CPPUNIT_ASSERT( nullptr == cellit->tex);
        --cellit;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(13), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(7), cellit->pointIdx[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit->pointIdx[4]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit->pointIdx[5]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(7), cellit[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), cellit[4]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit[5]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::POLY, static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(6), cellit.nbPoints());
        cellit--;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(9), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[3]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(6), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(5), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[3]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::QUAD, static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(4), cellit->nbPoints);
        cellit -= 2;
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(2), *cellit->offset);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit->pointIdx[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit->pointIdx[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(4), cellit->pointIdx[2]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(1), cellit[0]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(3), cellit[1]);
        CPPUNIT_ASSERT_EQUAL( static_cast<std::uint64_t>(4), cellit[2]);
        CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::CellType::TRIANGLE,
                              static_cast< ::fwData::Mesh::CellType >(*cellit->type));
        CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(3), cellit->nbPoints);

        mesh->setPoint(4, 45, 59, 48);
        auto itPt = mesh->begin< ::fwData::iterator::PointIterator >();
        itPt += 4;
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(45), itPt->point->x);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(59), itPt->point->y);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(48), itPt->point->z);
    }

    {
        const ::fwData::Mesh::Id NB_POINTS           = 60;
        const ::fwData::Mesh::Id NB_CELLS            = 59;
        const ::fwData::Mesh::CellType CELL_TYPE     = ::fwData::Mesh::CellType::TRIANGLE;
        const ::fwData::Mesh::Attributes EXTRA_ARRAY =
            ::fwData::Mesh::Attributes::POINT_NORMALS |
            ::fwData::Mesh::Attributes::POINT_COLORS |
            ::fwData::Mesh::Attributes::POINT_TEX_COORDS |
            ::fwData::Mesh::Attributes::CELL_NORMALS |
            ::fwData::Mesh::Attributes::CELL_COLORS |
            ::fwData::Mesh::Attributes::CELL_TEX_COORDS;

        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

        mesh->reserve(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
        const auto lock = mesh->lock();

        for (size_t i = 0; i < NB_POINTS; ++i)
        {
            const std::uint8_t val                                            = static_cast<uint8_t>(i);
            const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
            const float floatVal                                              = static_cast<float>(i);
            const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
            const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
            const size_t value                                                = 3*i;
            const auto id                                                     = mesh->pushPoint(
                static_cast<float>(value),
                static_cast<float>(value
                                   +1),
                static_cast<float>(value
                                   +2));
            mesh->setPointColor(id, color);
            mesh->setPointNormal(id, normal);
            mesh->setPointTexCoord(id, texCoords);
        }

        for (size_t i = 0; i < NB_CELLS; ++i)
        {
            const auto id = mesh->pushCell(i, i+1, i+2);

            const ::fwData::Mesh::ColorValueType val =
                static_cast< ::fwData::Mesh::ColorValueType >(i);
            const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
            const float floatVal                                              = static_cast<float>(i);
            const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
            const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
            mesh->setCellColor(id, color);
            mesh->setCellNormal(id, normal);
            mesh->setCellTexCoord(id, texCoords);
        }
        CPPUNIT_ASSERT_EQUAL(NB_POINTS, mesh->getNumberOfPoints());
        CPPUNIT_ASSERT_EQUAL(NB_CELLS, mesh->getNumberOfCells());
        CPPUNIT_ASSERT_EQUAL(NB_CELLS*3, mesh->getCellDataSize());
        CPPUNIT_ASSERT_EQUAL(mesh->getAllocatedSizeInBytes(), mesh->getDataSizeInBytes());
        const bool resizeMemory = mesh->adjustAllocatedMemory();
        CPPUNIT_ASSERT_EQUAL(false, resizeMemory);
    }
}

//------------------------------------------------------------------------------

void MeshTest::iteratorTest()
{
    const ::fwData::Mesh::Id NB_POINTS           = 60;
    const ::fwData::Mesh::Id NB_CELLS            = 59;
    const ::fwData::Mesh::CellType CELL_TYPE     = ::fwData::Mesh::CellType::TRIANGLE;
    const ::fwData::Mesh::Attributes EXTRA_ARRAY =
        ::fwData::Mesh::Attributes::POINT_NORMALS |
        ::fwData::Mesh::Attributes::POINT_COLORS |
        ::fwData::Mesh::Attributes::POINT_TEX_COORDS |
        ::fwData::Mesh::Attributes::CELL_NORMALS |
        ::fwData::Mesh::Attributes::CELL_COLORS |
        ::fwData::Mesh::Attributes::CELL_TEX_COORDS;

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

    mesh->resize(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock();

    for (size_t i = 0; i < NB_POINTS; ++i)
    {
        const std::uint8_t val                                            = static_cast<uint8_t>(i);
        const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
        const float floatVal                                              = static_cast<float>(i);
        const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
        const size_t value                                                = 3*i;
        mesh->setPoint(i, static_cast<float>(value), static_cast<float>(value+1), static_cast<float>(value+2));
        mesh->setPointColor(i, color);
        mesh->setPointNormal(i, normal);
        mesh->setPointTexCoord(i, texCoords);
    }

    for (size_t i = 0; i < NB_CELLS; ++i)
    {
        mesh->setCell(i, i, i+1, i+2);

        const ::fwData::Mesh::ColorValueType val =
            static_cast< ::fwData::Mesh::ColorValueType >(i);
        const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
        const float floatVal                                              = static_cast<float>(i);
        const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
        mesh->setCellColor(i, color);
        mesh->setCellNormal(i, normal);
        mesh->setCellTexCoord(i, texCoords);
    }

    {
        auto it          = mesh->begin< ::fwData::iterator::PointIterator >();
        const auto itEnd = mesh->end< ::fwData::iterator::PointIterator >();

        size_t count = 0;
        for (; it != itEnd; ++it)
        {
            ::fwData::iterator::Point p = *it->point;
            const float fValue = static_cast<float>(3*count);
            CPPUNIT_ASSERT_EQUAL(fValue, p.x);
            CPPUNIT_ASSERT_EQUAL(fValue+1, p.y);
            CPPUNIT_ASSERT_EQUAL(fValue+2, p.z);

            ::fwData::iterator::RGBA c = *it->rgba;
            const ::fwData::Mesh::ColorValueType cVal = static_cast< ::fwData::Mesh::ColorValueType >(count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.a);

            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType fVal = static_cast< ::fwData::Mesh::NormalValueType >(count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nx);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.ny);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nz);

            ::fwData::iterator::TexCoords uv = *it->tex;
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.u);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.v);

            ++count;
        }
    }

    {
        auto it            = mesh->end< ::fwData::iterator::PointIterator >()-1;
        const auto itBegin = mesh->begin< ::fwData::iterator::PointIterator >();

        size_t count = NB_POINTS-1;
        for (; it != itBegin; --it)
        {
            ::fwData::iterator::Point p = *it->point;
            const float fValue = static_cast<float>(3*count);
            CPPUNIT_ASSERT_EQUAL(fValue, p.x);
            CPPUNIT_ASSERT_EQUAL(fValue+1, p.y);
            CPPUNIT_ASSERT_EQUAL(fValue+2, p.z);

            ::fwData::iterator::RGBA c = *it->rgba;
            const ::fwData::Mesh::ColorValueType cVal = static_cast< ::fwData::Mesh::ColorValueType >(count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.a);

            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType fVal = static_cast< ::fwData::Mesh::NormalValueType >(count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nx);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.ny);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nz);

            ::fwData::iterator::TexCoords uv = *it->tex;
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.u);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.v);

            --count;
        }
    }

    {
        auto it          = mesh->begin< ::fwData::iterator::CellIterator >();
        const auto itEnd = mesh->end< ::fwData::iterator::CellIterator >();

        size_t count = 0;
        for (; it != itEnd; ++it)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(3), it.nbPoints());
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count), it[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count+1), it->pointIdx[1]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count+2), it->pointIdx[2]);

            ::fwData::iterator::RGBA c = *it->rgba;
            const ::fwData::Mesh::ColorValueType cVal = static_cast< ::fwData::Mesh::ColorValueType >(count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.a);

            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType fVal =
                static_cast< ::fwData::Mesh::NormalValueType >(count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nx);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.ny);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nz);

            ::fwData::iterator::TexCoords uv = *it->tex;
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.u);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.v);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(3), it->nbPoints);

            ++count;
        }
    }

    {
        auto it            = mesh->end< ::fwData::iterator::CellIterator >()-1;
        const auto itBegin = mesh->begin< ::fwData::iterator::CellIterator >();

        size_t count = 0;
        for (; it != itBegin; --it)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(3), it.nbPoints());
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(NB_CELLS-1-count), it[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(NB_CELLS-1-count+1), it->pointIdx[1]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(NB_CELLS-1-count+2), it->pointIdx[2]);

            ::fwData::iterator::RGBA c = *it->rgba;
            const ::fwData::Mesh::ColorValueType cVal = static_cast< ::fwData::Mesh::ColorValueType >(NB_CELLS-1-count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.a);

            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType fVal =
                static_cast< ::fwData::Mesh::NormalValueType >(NB_CELLS-1-count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nx);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.ny);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nz);

            ::fwData::iterator::TexCoords uv = *it->tex;
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.u);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.v);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(3), it->nbPoints);

            ++count;
        }
    }

    ::fwData::Mesh::csptr mesh2 = ::fwData::Mesh::copy(mesh);
    {
        auto it          = mesh2->begin< ::fwData::iterator::ConstPointIterator >();
        const auto itEnd = mesh2->end< ::fwData::iterator::ConstPointIterator >();

        float count = 0;
        for (; it != itEnd; ++it)
        {
            CPPUNIT_ASSERT_EQUAL(count++, it->point->x);
            CPPUNIT_ASSERT_EQUAL(count++, it->point->y);
            CPPUNIT_ASSERT_EQUAL(count++, it->point->z);
        }
    }

    {
        auto it            = mesh2->end< ::fwData::iterator::ConstCellIterator >()-1;
        const auto itBegin = mesh2->begin< ::fwData::iterator::ConstCellIterator >();

        size_t count = 0;
        for (; it != itBegin; --it)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(3), it.nbPoints());
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(NB_CELLS-1-count), it[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(NB_CELLS-1-count+1), it->pointIdx[1]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(NB_CELLS-1-count+2), it->pointIdx[2]);

            ::fwData::iterator::RGBA c = *it->rgba;
            const ::fwData::Mesh::ColorValueType cVal = static_cast< ::fwData::Mesh::ColorValueType >(NB_CELLS-1-count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), cVal, c.a);

            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType fVal =
                static_cast< ::fwData::Mesh::NormalValueType >(NB_CELLS-1-count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nx);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.ny);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, n.nz);

            ::fwData::iterator::TexCoords uv = *it->tex;
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.u);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), fVal, uv.v);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(3), it->nbPoints);

            ++count;
        }
    }

    ::fwData::Mesh::sptr mesh3 = ::fwData::Mesh::New();
    mesh3->resize(NB_POINTS, NB_CELLS, ::fwData::Mesh::CellType::QUAD, EXTRA_ARRAY);

    {
        auto it          = mesh3->begin< ::fwData::iterator::PointIterator >();
        const auto itEnd = mesh3->end< ::fwData::iterator::PointIterator >();

        size_t count = 0;
        for (; it != itEnd; ++it)
        {
            ::fwData::iterator::Point* p = it->point;
            p->x                         = static_cast<float>(3*count);
            p->y                         = static_cast<float>(3*count+1);
            p->z                         = static_cast<float>(3*count+2);

            ::fwData::iterator::RGBA* c = it->rgba;
            c->r                        = static_cast<std::uint8_t>(4*count);
            c->g                        = static_cast<std::uint8_t>(4*count+1);
            c->b                        = static_cast<std::uint8_t>(4*count+2);
            c->a                        = static_cast<std::uint8_t>(4*count+3);

            ::fwData::iterator::Normal* n = it->normal;
            n->nx                         = static_cast<float>(3*count+1);
            n->ny                         = static_cast<float>(3*count+2);
            n->nz                         = static_cast<float>(3*count+3);

            ::fwData::iterator::TexCoords* uv = it->tex;
            uv->u                             = static_cast<float>(2*count);
            uv->v                             = static_cast<float>(2*count+1);
            ++count;
        }
    }

    {
        auto it          = mesh3->begin< ::fwData::iterator::CellIterator >();
        const auto itEnd = mesh3->end< ::fwData::iterator::CellIterator >();

        size_t count = 0;
        for (; it != itEnd; ++it)
        {
            *it->type   = static_cast<std::uint8_t>(::fwData::Mesh::CellType::QUAD);
            *it->offset = 4*count;
            if (it != itEnd-1)
            {
                *(it+1)->offset = 4*(count+1);
            }

            for (size_t i = 0; i < 4; ++i)
            {
                it->pointIdx[i] = count + i;
            }

            ::fwData::iterator::RGBA& c = *it->rgba;
            c.r                         = static_cast<std::uint8_t>(4*count);
            c.g                         = static_cast<std::uint8_t>(4*count+1);
            c.b                         = static_cast<std::uint8_t>(4*count+2);
            c.a                         = static_cast<std::uint8_t>(4*count+3);

            ::fwData::iterator::Normal& n = *it->normal;
            n.nx                          = static_cast<float>(3*count+1);
            n.ny                          = static_cast<float>(3*count+2);
            n.nz                          = static_cast<float>(3*count+3);

            ::fwData::iterator::TexCoords& uv = *it->tex;
            uv.u                              = static_cast<float>(2*count);
            uv.v                              = static_cast<float>(2*count+1);
            ++count;
        }
    }

    {
        auto it          = mesh3->begin< ::fwData::iterator::ConstPointIterator >();
        const auto itEnd = mesh3->end< ::fwData::iterator::ConstPointIterator >();

        size_t count = 0;
        for (; it != itEnd; ++it)
        {
            ::fwData::iterator::Point p = *it->point;
            const float fValue = static_cast<float>(3*count);
            CPPUNIT_ASSERT_EQUAL(fValue, p.x);
            CPPUNIT_ASSERT_EQUAL(fValue+1, p.y);
            CPPUNIT_ASSERT_EQUAL(fValue+2, p.z);

            ::fwData::iterator::RGBA c = *it->rgba;
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count, static_cast<size_t>(c.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count+1, static_cast<size_t>(c.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count+2, static_cast<size_t>(c.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count+3, static_cast<size_t>(c.a));

            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType nVal = static_cast< ::fwData::Mesh::NormalValueType >(3*count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), nVal+1, n.nx);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), nVal+2, n.ny);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), nVal+3, n.nz);

            ::fwData::iterator::TexCoords uv = *it->tex;
            const ::fwData::Mesh::NormalValueType uvVal = static_cast< ::fwData::Mesh::NormalValueType >(2*count);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), uvVal, uv.u);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), uvVal+1, uv.v);

            ++count;
        }
    }

    {
        auto it          = mesh3->begin< ::fwData::iterator::ConstCellIterator >();
        const auto itEnd = mesh3->end< ::fwData::iterator::ConstCellIterator >();

        size_t count = 0;
        for (; it != itEnd; ++it)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(4), it.nbPoints());
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count), it->pointIdx[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count+1), it->pointIdx[1]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count+2), it->pointIdx[2]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(
                                             count), static_cast<std::uint64_t>(count+3), it->pointIdx[3]);

            ::fwData::iterator::RGBA c = *it->rgba;
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count, static_cast<size_t>(c.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count+1, static_cast<size_t>(c.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count+2, static_cast<size_t>(c.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), 4*count+3, static_cast<size_t>(c.a));
            ::fwData::iterator::Normal n = *it->normal;
            const ::fwData::Mesh::NormalValueType nVal =
                static_cast< ::fwData::Mesh::NormalValueType >(3*count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), nVal+1, n.nx);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), nVal+2, n.ny);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), nVal+3, n.nz);

            ::fwData::iterator::TexCoords uv = *it->tex;
            const ::fwData::Mesh::NormalValueType uvVal =
                static_cast< ::fwData::Mesh::NormalValueType >(2*count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), uvVal, uv.u);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), uvVal+1, uv.v);

            CPPUNIT_ASSERT_EQUAL_MESSAGE("iteration: " + std::to_string(count), static_cast<size_t>(4), it->nbPoints);

            ++count;
        }
    }

    {

    }

}
} //namespace ut
} //namespace fwData

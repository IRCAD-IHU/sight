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

#include "MeshTest.hpp"

#include <fwVtkIO/helper/Mesh.hpp>
#include <fwVtkIO/MeshReader.hpp>
#include <fwVtkIO/MeshWriter.hpp>
#include <fwVtkIO/ObjMeshReader.hpp>
#include <fwVtkIO/ObjMeshWriter.hpp>
#include <fwVtkIO/PlyMeshReader.hpp>
#include <fwVtkIO/PlyMeshWriter.hpp>
#include <fwVtkIO/StlMeshReader.hpp>
#include <fwVtkIO/StlMeshWriter.hpp>
#include <fwVtkIO/VtpMeshReader.hpp>
#include <fwVtkIO/VtpMeshWriter.hpp>

#include <core/tools/NumericRoundCast.hxx>
#include <core/tools/System.hpp>

#include <data/reflection/visitor/CompareObjects.hpp>
#include <data/tools/Mesh.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Mesh.hpp>

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTexturedSphereSource.h>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwVtkIO::ut::MeshTest );

namespace fwVtkIO
{
namespace ut
{

//-----------------------------------------------------------------------------

void compare(data::Object::sptr objRef, data::Object::sptr objComp)
{
    data::reflection::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(data::reflection::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    for( data::reflection::visitor::CompareObjects::PropsMapType::value_type prop :  (*props) )
    {
        SLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    CPPUNIT_ASSERT_MESSAGE("Object Not equal", props->size() == 0 );
}

//------------------------------------------------------------------------------

void MeshTest::setUp()
{
    // Set up context before running a test.

    std::srand(core::tools::numericRoundCast<unsigned int>(std::time(NULL)));
}

//------------------------------------------------------------------------------

void MeshTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MeshTest::testMeshToVtk()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkTexturedSphereSource > source = vtkSmartPointer< vtkTexturedSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfPoints());

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    data::tools::Mesh::generatePointNormals(mesh1);
    data::tools::Mesh::generateCellNormals(mesh1);
    data::tools::Mesh::colorizeMeshPoints(mesh1);
    data::tools::Mesh::colorizeMeshCells(mesh1);

    CPPUNIT_ASSERT( mesh1->getNumberOfCells() );
    CPPUNIT_ASSERT( mesh1->getNumberOfPoints() );
    CPPUNIT_ASSERT( mesh1->hasPointNormals() );
    CPPUNIT_ASSERT( mesh1->hasCellNormals() );
    CPPUNIT_ASSERT( mesh1->hasPointColors() );
    CPPUNIT_ASSERT( mesh1->hasCellColors() );
    CPPUNIT_ASSERT( mesh1->hasPointTexCoords() );

    const vtkSmartPointer< vtkPolyData > vtkMesh = vtkSmartPointer< vtkPolyData >::New();
    ::fwVtkIO::helper::Mesh::toVTKMesh( mesh1, vtkMesh);
    CPPUNIT_ASSERT( vtkMesh );

    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfVerts(), vtkMesh->GetNumberOfVerts());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfLines(), vtkMesh->GetNumberOfLines());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfPolys(), vtkMesh->GetNumberOfPolys());
    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfStrips(), vtkMesh->GetNumberOfStrips());

    const data::Mesh::sptr mesh2 = data::Mesh::New();
    CPPUNIT_ASSERT( mesh2 );
    ::fwVtkIO::helper::Mesh::fromVTKMesh(vtkMesh, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testMeshToGrid()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkTexturedSphereSource > source = vtkSmartPointer< vtkTexturedSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfPoints());

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    data::tools::Mesh::generatePointNormals(mesh1);
    data::tools::Mesh::generateCellNormals(mesh1);
    data::tools::Mesh::colorizeMeshPoints(mesh1);
    data::tools::Mesh::colorizeMeshCells(mesh1);

    CPPUNIT_ASSERT( mesh1->getNumberOfCells() );
    CPPUNIT_ASSERT( mesh1->getNumberOfPoints() );
    CPPUNIT_ASSERT( mesh1->hasPointNormals() );
    CPPUNIT_ASSERT( mesh1->hasCellNormals() );
    CPPUNIT_ASSERT( mesh1->hasPointColors() );
    CPPUNIT_ASSERT( mesh1->hasCellColors() );
    CPPUNIT_ASSERT( mesh1->hasPointTexCoords() );
    const vtkSmartPointer< vtkUnstructuredGrid > vtkGrid = vtkSmartPointer< vtkUnstructuredGrid >::New();
    ::fwVtkIO::helper::Mesh::toVTKGrid( mesh1, vtkGrid);
    CPPUNIT_ASSERT( vtkGrid );

    CPPUNIT_ASSERT_EQUAL(poly_source->GetNumberOfPoints(), vtkGrid->GetNumberOfPoints());

    const data::Mesh::sptr mesh2 = data::Mesh::New();
    CPPUNIT_ASSERT( mesh2 );
    ::fwVtkIO::helper::Mesh::fromVTKGrid(vtkGrid, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testSyntheticMesh()
{
    {
        const data::Mesh::sptr mesh1 = data::Mesh::New();
        utestData::generator::Mesh::generateTriangleQuadMesh(mesh1);
        data::tools::Mesh::shakePoint(mesh1);
        mesh1->adjustAllocatedMemory();

        const vtkSmartPointer< vtkPolyData > poly = vtkSmartPointer< vtkPolyData >::New();
        ::fwVtkIO::helper::Mesh::toVTKMesh( mesh1, poly);
        CPPUNIT_ASSERT( poly );

        data::Mesh::sptr mesh2 = data::Mesh::New();
        ::fwVtkIO::helper::Mesh::fromVTKMesh(poly, mesh2);

        compare(mesh1, mesh2);
    }
    {
        const data::Mesh::sptr mesh1 = data::Mesh::New();
        utestData::generator::Mesh::generateTriangleQuadMesh(mesh1);
        data::tools::Mesh::shakePoint(mesh1);
        data::tools::Mesh::colorizeMeshPoints(mesh1);
        data::tools::Mesh::colorizeMeshCells(mesh1);
        data::tools::Mesh::generatePointNormals(mesh1);
        data::tools::Mesh::generateCellNormals(mesh1);
        mesh1->adjustAllocatedMemory();

        const vtkSmartPointer< vtkPolyData > poly = vtkSmartPointer< vtkPolyData >::New();
        ::fwVtkIO::helper::Mesh::toVTKMesh( mesh1, poly);
        CPPUNIT_ASSERT( poly );

        data::Mesh::sptr mesh2 = data::Mesh::New();
        ::fwVtkIO::helper::Mesh::fromVTKMesh(poly, mesh2);

        compare(mesh1, mesh2);
    }
}

//------------------------------------------------------------------------------

void MeshTest::testExportImportSyntheticMesh()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleQuadMesh(mesh1);
    data::tools::Mesh::shakePoint(mesh1);
    data::tools::Mesh::colorizeMeshPoints(mesh1);
    data::tools::Mesh::colorizeMeshCells(mesh1);
    data::tools::Mesh::generatePointNormals(mesh1);
    data::tools::Mesh::generateCellNormals(mesh1);

    mesh1->adjustAllocatedMemory();

    const std::filesystem::path testFile = core::tools::System::getTemporaryFolder() /
                                           "testExportImportSyntheticMesh.vtk";

    const ::fwVtkIO::MeshWriter::sptr writer = ::fwVtkIO::MeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    writer->write();
    CPPUNIT_ASSERT(std::filesystem::exists(testFile));

    const data::Mesh::sptr mesh2             = data::Mesh::New();
    const ::fwVtkIO::MeshReader::sptr reader = ::fwVtkIO::MeshReader::New();
    reader->setObject(mesh2);
    reader->setFile(testFile);
    reader->read();

    compare(mesh1, mesh2);

    const bool suppr = std::filesystem::remove(testFile);
    CPPUNIT_ASSERT(suppr);
}

//------------------------------------------------------------------------------

void MeshTest::testPointCloud()
{
    const data::Mesh::Size NB_POINTS = static_cast< data::Mesh::Size >(100 + rand()%1000);

    const data::Mesh::sptr mesh1 = data::Mesh::New();

    mesh1->reserve(NB_POINTS, NB_POINTS, data::Mesh::CellType::POINT);

    const auto dumpLock = mesh1->lock();

    for (data::Mesh::Size i = 0; i < NB_POINTS; ++i)
    {
        data::Mesh::PointValueType point[3];
        point[0] = (static_cast<float>(rand()%1000) - 500.f) / 3.f;
        point[1] = (static_cast<float>(rand()%1000) - 500.f) / 3.f;
        point[2] = (static_cast<float>(rand()%1000) - 500.f) / 3.f;
        mesh1->pushPoint(point);
        mesh1->pushCell(i);
    }

    const vtkSmartPointer< vtkPolyData > poly = vtkSmartPointer< vtkPolyData >::New();
    ::fwVtkIO::helper::Mesh::toVTKMesh( mesh1, poly);
    CPPUNIT_ASSERT( poly );

    const data::Mesh::sptr mesh2 = data::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly, mesh2);

    CPPUNIT_ASSERT_EQUAL(NB_POINTS, mesh2->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL(NB_POINTS, mesh2->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(NB_POINTS, mesh2->getCellDataSize());
    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testMeshUpdatePoints()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfPoints());

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    // modify mesh vertex position
    data::tools::Mesh::shakePoint(mesh1);

    ::fwVtkIO::helper::Mesh::updatePolyDataPoints(poly_source, mesh1);

    const data::Mesh::sptr mesh2 = data::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testMeshUpdateColors()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfPoints());

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    data::tools::Mesh::colorizeMeshPoints(mesh1);
    data::tools::Mesh::colorizeMeshCells(mesh1);

    ::fwVtkIO::helper::Mesh::updatePolyDataPointColor(poly_source, mesh1);
    ::fwVtkIO::helper::Mesh::updatePolyDataCellColor(poly_source, mesh1);

    const data::Mesh::sptr mesh2 = data::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testMeshUpdateNormals()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfPoints());

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    data::tools::Mesh::generatePointNormals(mesh1);
    data::tools::Mesh::generateCellNormals(mesh1);
    data::tools::Mesh::shakePointNormals(mesh1);
    data::tools::Mesh::shakeCellNormals(mesh1);

    ::fwVtkIO::helper::Mesh::updatePolyDataPointNormals(poly_source, mesh1);
    ::fwVtkIO::helper::Mesh::updatePolyDataCellNormals(poly_source, mesh1);

    const data::Mesh::sptr mesh2 = data::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testGridUpdatePoints()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfPoints());

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    vtkSmartPointer< vtkUnstructuredGrid > vtkGrid = vtkSmartPointer< vtkUnstructuredGrid >::New();
    ::fwVtkIO::helper::Mesh::toVTKGrid( mesh1, vtkGrid);
    CPPUNIT_ASSERT( vtkGrid );

    // modify mesh vertex position
    data::tools::Mesh::shakePoint(mesh1);
    ::fwVtkIO::helper::Mesh::updateGridPoints(vtkGrid, mesh1);

    const data::Mesh::sptr mesh2 = data::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKGrid(vtkGrid, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testGridUpdateColors()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    vtkSmartPointer< vtkUnstructuredGrid > vtkGrid = vtkSmartPointer< vtkUnstructuredGrid >::New();
    ::fwVtkIO::helper::Mesh::toVTKGrid( mesh1, vtkGrid);
    CPPUNIT_ASSERT( vtkGrid );

    data::tools::Mesh::colorizeMeshPoints(mesh1);
    data::tools::Mesh::colorizeMeshCells(mesh1);

    ::fwVtkIO::helper::Mesh::updateGridPointColor(vtkGrid, mesh1);
    ::fwVtkIO::helper::Mesh::updateGridCellColor(vtkGrid, mesh1);

    const data::Mesh::sptr mesh2 = data::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKGrid(vtkGrid, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testGridUpdateNormals()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    CPPUNIT_ASSERT( mesh1 );

    const vtkSmartPointer< vtkSphereSource > source = vtkSmartPointer< vtkSphereSource >::New();
    source->SetThetaResolution(50);
    source->SetPhiResolution(50);
    source->SetRadius(2);
    source->Update();
    const vtkSmartPointer< vtkPolyData > poly_source = source->GetOutput();

    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL(static_cast< data::Mesh::Size>(0), mesh1->getNumberOfPoints());

    ::fwVtkIO::helper::Mesh::fromVTKMesh(poly_source, mesh1);

    data::tools::Mesh::generatePointNormals(mesh1);
    data::tools::Mesh::generateCellNormals(mesh1);

    CPPUNIT_ASSERT( mesh1->getNumberOfCells() );
    CPPUNIT_ASSERT( mesh1->getNumberOfPoints() );

    vtkSmartPointer< vtkUnstructuredGrid > vtkGrid = vtkSmartPointer< vtkUnstructuredGrid >::New();
    ::fwVtkIO::helper::Mesh::toVTKGrid( mesh1, vtkGrid);
    CPPUNIT_ASSERT( vtkGrid );

    ::fwVtkIO::helper::Mesh::updateGridPoints(vtkGrid, mesh1);
    ::fwVtkIO::helper::Mesh::updateGridPointNormals(vtkGrid, mesh1);
    ::fwVtkIO::helper::Mesh::updateGridCellNormals(vtkGrid, mesh1);

    const data::Mesh::sptr mesh2 = data::Mesh::New();
    ::fwVtkIO::helper::Mesh::fromVTKGrid(vtkGrid, mesh2);

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void MeshTest::testReadVtkFile()
{
    const std::filesystem::path testFile(utestData::Data::dir() / ("sight/mesh/vtk/sphere.vtk"));
    CPPUNIT_ASSERT_MESSAGE("The file '" + testFile.string() + "' does not exist",
                           std::filesystem::exists(testFile));

    data::Mesh::sptr mesh = data::Mesh::New();

    ::fwVtkIO::MeshReader::sptr vtkReader = ::fwVtkIO::MeshReader::New();

    vtkReader->setObject(mesh);
    vtkReader->setFile(testFile);

    CPPUNIT_ASSERT_NO_THROW(vtkReader->read());

    CPPUNIT_ASSERT(mesh->getNumberOfCells() == 720);
    CPPUNIT_ASSERT(mesh->getNumberOfPoints() == 362);

}

//------------------------------------------------------------------------------

void MeshTest::testReadVtpFile()
{
    const std::filesystem::path testFile(utestData::Data::dir() / ("sight/mesh/vtp/sphere.vtp"));
    CPPUNIT_ASSERT_MESSAGE("The file '" + testFile.string() + "' does not exist",
                           std::filesystem::exists(testFile));

    data::Mesh::sptr mesh = data::Mesh::New();

    ::fwVtkIO::VtpMeshReader::sptr vtkReader = ::fwVtkIO::VtpMeshReader::New();

    vtkReader->setObject(mesh);
    vtkReader->setFile(testFile);

    CPPUNIT_ASSERT_NO_THROW(vtkReader->read());

    CPPUNIT_ASSERT(mesh->getNumberOfCells() == 720);
    CPPUNIT_ASSERT(mesh->getNumberOfPoints() == 362);
}

//------------------------------------------------------------------------------

void MeshTest::testReadObjFile()
{
    const std::filesystem::path testFile(utestData::Data::dir() / ("sight/mesh/obj/sphere.obj"));
    CPPUNIT_ASSERT_MESSAGE("The file '" + testFile.string() + "' does not exist",
                           std::filesystem::exists(testFile));

    data::Mesh::sptr mesh = data::Mesh::New();

    ::fwVtkIO::ObjMeshReader::sptr vtkReader = ::fwVtkIO::ObjMeshReader::New();

    vtkReader->setObject(mesh);
    vtkReader->setFile(testFile);

    CPPUNIT_ASSERT_NO_THROW(vtkReader->read());

    CPPUNIT_ASSERT(mesh->getNumberOfCells() == 720);
    CPPUNIT_ASSERT(mesh->getNumberOfPoints() == 362);
}

//------------------------------------------------------------------------------

void MeshTest::testReadPlyFile()
{
    const std::filesystem::path testFile(utestData::Data::dir() / ("sight/mesh/ply/sphere.ply"));
    CPPUNIT_ASSERT_MESSAGE("The file '" + testFile.string() + "' does not exist",
                           std::filesystem::exists(testFile));

    data::Mesh::sptr mesh = data::Mesh::New();

    ::fwVtkIO::PlyMeshReader::sptr vtkReader = ::fwVtkIO::PlyMeshReader::New();

    vtkReader->setObject(mesh);
    vtkReader->setFile(testFile);

    CPPUNIT_ASSERT_NO_THROW(vtkReader->read());

    CPPUNIT_ASSERT(mesh->getNumberOfCells() == 720);
    CPPUNIT_ASSERT(mesh->getNumberOfPoints() == 362);
}

//------------------------------------------------------------------------------

void MeshTest::testReadStlFile()
{
    const std::filesystem::path testFile(utestData::Data::dir() / ("sight/mesh/stl/sphere.stl"));
    CPPUNIT_ASSERT_MESSAGE("The file '" + testFile.string() + "' does not exist",
                           std::filesystem::exists(testFile));

    data::Mesh::sptr mesh = data::Mesh::New();

    ::fwVtkIO::StlMeshReader::sptr vtkReader = ::fwVtkIO::StlMeshReader::New();

    vtkReader->setObject(mesh);
    vtkReader->setFile(testFile);

    CPPUNIT_ASSERT_NO_THROW(vtkReader->read());

    CPPUNIT_ASSERT(mesh->getNumberOfCells() == 720);
    CPPUNIT_ASSERT(mesh->getNumberOfPoints() == 362);
}

//------------------------------------------------------------------------------

void MeshTest::testWriteVtkFile()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleMesh(mesh1);
    data::tools::Mesh::shakePoint(mesh1);
    data::tools::Mesh::colorizeMeshPoints(mesh1);
    data::tools::Mesh::colorizeMeshCells(mesh1);
    data::tools::Mesh::generatePointNormals(mesh1);
    data::tools::Mesh::generateCellNormals(mesh1);

    mesh1->adjustAllocatedMemory();

    const std::filesystem::path testFile = core::tools::System::getTemporaryFolder() /
                                           "vtkTestMesh.vtk";

    const ::fwVtkIO::MeshWriter::sptr writer = ::fwVtkIO::MeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(testFile));
}

//------------------------------------------------------------------------------

void MeshTest::testWriteVtpFile()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleMesh(mesh1);
    data::tools::Mesh::shakePoint(mesh1);
    data::tools::Mesh::colorizeMeshPoints(mesh1);
    data::tools::Mesh::colorizeMeshCells(mesh1);
    data::tools::Mesh::generatePointNormals(mesh1);
    data::tools::Mesh::generateCellNormals(mesh1);

    mesh1->adjustAllocatedMemory();

    const std::filesystem::path testFile = core::tools::System::getTemporaryFolder() /
                                           "vtpTestMesh.vtp";

    const ::fwVtkIO::VtpMeshWriter::sptr writer = ::fwVtkIO::VtpMeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(testFile));
}

//------------------------------------------------------------------------------

void MeshTest::testWriteObjFile()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleMesh(mesh1);
    data::tools::Mesh::shakePoint(mesh1);
    data::tools::Mesh::colorizeMeshPoints(mesh1);
    data::tools::Mesh::colorizeMeshCells(mesh1);
    data::tools::Mesh::generatePointNormals(mesh1);
    data::tools::Mesh::generateCellNormals(mesh1);

    mesh1->adjustAllocatedMemory();

    const std::filesystem::path testFile = core::tools::System::getTemporaryFolder() /
                                           "objTestMesh.obj";

    const ::fwVtkIO::ObjMeshWriter::sptr writer = ::fwVtkIO::ObjMeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(testFile));
}

//------------------------------------------------------------------------------

void MeshTest::testWritePlyFile()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleMesh(mesh1);
    data::tools::Mesh::shakePoint(mesh1);
    data::tools::Mesh::colorizeMeshPoints(mesh1);
    data::tools::Mesh::colorizeMeshCells(mesh1);
    data::tools::Mesh::generatePointNormals(mesh1);
    data::tools::Mesh::generateCellNormals(mesh1);

    mesh1->adjustAllocatedMemory();

    const std::filesystem::path testFile = core::tools::System::getTemporaryFolder() /
                                           "plyTestMesh.ply";

    const ::fwVtkIO::PlyMeshWriter::sptr writer = ::fwVtkIO::PlyMeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(testFile));
}

//------------------------------------------------------------------------------

void MeshTest::testWriteStlFile()
{
    const data::Mesh::sptr mesh1 = data::Mesh::New();
    utestData::generator::Mesh::generateTriangleMesh(mesh1);
    data::tools::Mesh::shakePoint(mesh1);
    data::tools::Mesh::colorizeMeshPoints(mesh1);
    data::tools::Mesh::colorizeMeshCells(mesh1);
    data::tools::Mesh::generatePointNormals(mesh1);
    data::tools::Mesh::generateCellNormals(mesh1);

    mesh1->adjustAllocatedMemory();

    const std::filesystem::path testFile = core::tools::System::getTemporaryFolder() /
                                           "stlTestMesh.stl";

    const ::fwVtkIO::StlMeshWriter::sptr writer = ::fwVtkIO::StlMeshWriter::New();
    writer->setObject(mesh1);
    writer->setFile(testFile);
    CPPUNIT_ASSERT_NO_THROW(writer->write());
    CPPUNIT_ASSERT(std::filesystem::exists(testFile));
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwVtkIO

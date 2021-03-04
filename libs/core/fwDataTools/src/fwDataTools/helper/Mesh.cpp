/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwDataTools/helper/Mesh.hpp"

#include <fwMath/MeshFunctions.hpp>

namespace fwDataTools
{

namespace helper
{

#define POINT_REALLOC_STEP 1000
#define CELL_REALLOC_STEP 1000
#define CELLDATA_REALLOC_STEP 1000

Mesh::Mesh( ::fwData::Mesh::sptr mesh ) :
    m_mesh(mesh)
{
    FW_DEPRECATED_MSG("::fwDataTools::helper::Mesh is no longer supported, the methods have been moved to "
                      "::fwData::Mesh", "22.0")
    SLM_ASSERT("Mesh ptr is null.", mesh);
    this->updateLock();
}

//-----------------------------------------------------------------------------

Mesh::~Mesh()
{
}

//-----------------------------------------------------------------------------

void Mesh::updateLock()
{
    SLM_ASSERT("Mesh ptr is null.", m_mesh);
    m_mesh->getPointsArray()->setNumberOfComponents(3);
    m_helperPoints          = ::fwDataTools::helper::Array::New(m_mesh->getPointsArray());
    m_helperCellTypes       = ::fwDataTools::helper::Array::New(m_mesh->getCellTypesArray());
    m_helperCellData        = ::fwDataTools::helper::Array::New(m_mesh->getCellDataArray());
    m_helperCellDataOffsets = ::fwDataTools::helper::Array::New(m_mesh->getCellDataOffsetsArray());

    ::fwData::Array::sptr pointColors    = m_mesh->getPointColorsArray();
    ::fwData::Array::sptr cellColors     = m_mesh->getCellColorsArray();
    ::fwData::Array::sptr pointNormals   = m_mesh->getPointNormalsArray();
    ::fwData::Array::sptr cellNormals    = m_mesh->getCellNormalsArray();
    ::fwData::Array::sptr pointTexCoords = m_mesh->getPointTexCoordsArray();
    ::fwData::Array::sptr cellTexCoords  = m_mesh->getCellTexCoordsArray();

    if(pointColors)
    {
        m_helperPointColors = ::fwDataTools::helper::Array::New(pointColors);
    }
    if(cellColors)
    {
        m_helperCellColors = ::fwDataTools::helper::Array::New(cellColors);
    }
    if(pointNormals)
    {
        m_helperPointNormals = ::fwDataTools::helper::Array::New(pointNormals);
    }
    if(cellNormals)
    {
        m_helperCellNormals = ::fwDataTools::helper::Array::New(cellNormals);
    }
    if(pointTexCoords)
    {
        m_helperPointTexCoords = ::fwDataTools::helper::Array::New(pointTexCoords);
    }
    if(cellTexCoords)
    {
        m_helperCellTexCoords = ::fwDataTools::helper::Array::New(cellTexCoords);
    }
}

//-----------------------------------------------------------------------------

::fwData::Mesh::PointId Mesh::insertNextPoint(const ::fwData::Mesh::PointValueType p[3])
{
    ::fwData::Mesh::Size nbPoints = m_mesh->getNumberOfPoints();
    ::fwData::Array::sptr points  = m_mesh->getPointsArray();
    size_t allocatedPts = points->empty() ? 0 : points->getSize().at(0);
    if( allocatedPts <= nbPoints )
    {
        points->resize({allocatedPts + POINT_REALLOC_STEP}, true);
    }
    m_helperPoints->setItem({nbPoints}, p);
    m_mesh->setNumberOfPoints(nbPoints+1);
    return nbPoints;
}

//------------------------------------------------------------------------------

::fwData::Mesh::PointId Mesh::insertNextPoint(::fwData::Mesh::PointValueType x,
                                              ::fwData::Mesh::PointValueType y,
                                              ::fwData::Mesh::PointValueType z)
{
    const ::fwData::Mesh::PointValueType p[3] = {x, y, z};
    return this->insertNextPoint(p);
}

//------------------------------------------------------------------------------

void Mesh::setPoint(::fwData::Mesh::PointId id, const ::fwData::Mesh::PointValueType p[3])
{
    m_helperPoints->setItem( {id}, p);
}

//------------------------------------------------------------------------------

void Mesh::setPoint(::fwData::Mesh::PointId id,
                    ::fwData::Mesh::PointValueType x,
                    ::fwData::Mesh::PointValueType y,
                    ::fwData::Mesh::PointValueType z )
{
    const ::fwData::Mesh::PointValueType p[3] = {x, y, z};
    this->setPoint(id, p);
}

//------------------------------------------------------------------------------

void Mesh::setPointColor(::fwData::Mesh::PointId id, const ::fwData::Mesh::ColorValueType c[4])
{
    m_helperPointColors->setItem({ id }, c);
}

//------------------------------------------------------------------------------

void Mesh::setCellColor(::fwData::Mesh::CellId id, const ::fwData::Mesh::ColorValueType c[4])
{
    m_helperCellColors->setItem({ id }, c);
}

//------------------------------------------------------------------------------

void Mesh::setPointNormal(::fwData::Mesh::PointId id, const ::fwData::Mesh::NormalValueType n[3])
{
    m_helperPointNormals->setItem({ id }, n);
}

//------------------------------------------------------------------------------

void Mesh::setCellNormal(::fwData::Mesh::CellId id, const ::fwData::Mesh::NormalValueType n[3])
{
    m_helperCellNormals->setItem({ id }, n);
}

//------------------------------------------------------------------------------

void Mesh::setPointTexCoord(::fwData::Mesh::PointId id, const ::fwData::Mesh::TexCoordValueType t[2])
{
    m_helperPointTexCoords->setItem({ id }, t);
}

//------------------------------------------------------------------------------

void Mesh::setCellTexCoord(::fwData::Mesh::CellId id, const ::fwData::Mesh::TexCoordValueType t[2])
{
    m_helperCellTexCoords->setItem({ id }, t);
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellId Mesh::insertNextCell(::fwData::Mesh::CellTypesEnum type,
                                            const ::fwData::Mesh::CellId* cell,
                                            size_t nb)
{
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'NO_CELL'",
               type != ::fwData::Mesh::NO_CELL || nb == 0);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'POINT'",
               type != ::fwData::Mesh::POINT || nb == 1);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'EDGE'",
               type != ::fwData::Mesh::EDGE || nb == 2);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'TRIANGLE'",
               type != ::fwData::Mesh::TRIANGLE || nb == 3);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'QUAD'",
               type != ::fwData::Mesh::QUAD || nb == 4);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'TETRA'",
               type != ::fwData::Mesh::TETRA || nb == 4);
    SLM_ASSERT("Bad number of points ("<< nb << ") for cell type: 'POLY'",
               type != ::fwData::Mesh::POLY || nb > 4);

    ::fwData::Mesh::Size cellsDataSize    = m_mesh->getCellDataSize();
    ::fwData::Mesh::Size nbCells          = m_mesh->getNumberOfCells();
    ::fwData::Array::sptr cellTypes       = m_mesh->getCellTypesArray();
    ::fwData::Array::sptr cellDataOffsets = m_mesh->getCellDataOffsetsArray();
    ::fwData::Array::sptr cellData        = m_mesh->getCellDataArray();

    size_t allocatedCellTypes       = cellTypes->empty() ? 0 : cellTypes->getSize().at(0);
    size_t allocatedCellDataOffsets = cellDataOffsets->empty() ? 0 : cellDataOffsets->getSize().at(0);

    if( allocatedCellTypes <= nbCells )
    {
        cellTypes->resize({allocatedCellTypes + CELL_REALLOC_STEP}, true);
    }
    if( allocatedCellDataOffsets <= nbCells )
    {
        cellDataOffsets->resize({allocatedCellDataOffsets + CELL_REALLOC_STEP}, true);
    }

    size_t allocatedCellData = cellData->empty() ? 0 : cellData->getSize().at(0);

    if( allocatedCellData <= cellsDataSize + nb )
    {
        cellData->resize({allocatedCellData + CELLDATA_REALLOC_STEP}, true);
    }

    const ::fwData::Mesh::CellTypes t[1] = {static_cast< ::fwData::Mesh::CellTypes >(type)};
    m_helperCellTypes->setItem({nbCells}, t);

    ::fwData::Mesh::CellId* buf = reinterpret_cast< ::fwData::Mesh::CellId* >(
        m_helperCellData->getBufferPtr({cellsDataSize}, 0, sizeof(::fwData::Mesh::CellId))
        );
    std::copy(cell, cell+nb, buf);

    const ::fwData::Mesh::CellId id[1] = {cellsDataSize};
    m_helperCellDataOffsets->setItem({nbCells}, id);

    cellsDataSize += nb;
    m_mesh->setCellDataSize(cellsDataSize);
    m_mesh->setNumberOfCells(nbCells + 1);
    return nbCells;
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellId Mesh::insertNextCell(::fwData::Mesh::PointId p)
{
    ::fwData::Mesh::PointId point[1] = {p};
    return this->insertNextCell(::fwData::Mesh::POINT, point, 1);
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellId Mesh::insertNextCell(::fwData::Mesh::PointId p1,
                                            ::fwData::Mesh::PointId p2)
{
    ::fwData::Mesh::PointId p[2] = {p1, p2};
    return this->insertNextCell(::fwData::Mesh::EDGE, p, 2);
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellId Mesh::insertNextCell(::fwData::Mesh::PointId p1,
                                            ::fwData::Mesh::PointId p2,
                                            ::fwData::Mesh::PointId p3)
{
    ::fwData::Mesh::PointId p[3] = {p1, p2, p3};
    return this->insertNextCell(::fwData::Mesh::TRIANGLE, p, 3);
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellId Mesh::insertNextCell(::fwData::Mesh::PointId p1,
                                            ::fwData::Mesh::PointId p2,
                                            ::fwData::Mesh::PointId p3,
                                            ::fwData::Mesh::PointId p4,
                                            ::fwData::Mesh::CellTypesEnum type)
{
    ::fwData::Mesh::PointId p[4] = {p1, p2, p3, p4};

    return this->insertNextCell(type, p, 4);
}

//------------------------------------------------------------------------------

::fwData::Mesh::PointsMultiArrayType Mesh::getPoints() const
{
    return ::fwData::Mesh::PointsMultiArrayType(
        static_cast< ::fwData::Mesh::PointsMultiArrayType::element* >(m_helperPoints->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][3]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellTypesMultiArrayType Mesh::getCellTypes() const
{
    return ::fwData::Mesh::CellTypesMultiArrayType(
        static_cast< ::fwData::Mesh::CellTypesMultiArrayType::element*> (m_helperCellTypes->getBuffer()),
        boost::extents[m_mesh->getNumberOfCells()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellDataMultiArrayType Mesh::getCellData() const
{
    return ::fwData::Mesh::CellDataMultiArrayType(
        static_cast< ::fwData::Mesh::CellDataMultiArrayType::element* >(m_helperCellData->getBuffer()),
        ::boost::extents[m_mesh->getCellDataSize()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellDataOffsetsMultiArrayType Mesh::getCellDataOffsets() const
{
    return ::fwData::Mesh::CellDataOffsetsMultiArrayType(
        static_cast< ::fwData::Mesh::CellDataOffsetsMultiArrayType::element*>(m_helperCellDataOffsets->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::PointColorsMultiArrayType Mesh::getPointColors() const
{
    ::fwData::Array::sptr pointColors = m_mesh->getPointColorsArray();
    return ::fwData::Mesh::PointColorsMultiArrayType(
        static_cast< ::fwData::Mesh::PointColorsMultiArrayType::element*>(m_helperPointColors->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][pointColors->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellColorsMultiArrayType Mesh::getCellColors() const
{
    ::fwData::Array::sptr cellColors = m_mesh->getCellColorsArray();
    return ::fwData::Mesh::CellColorsMultiArrayType(
        static_cast< ::fwData::Mesh::CellColorsMultiArrayType::element*>(m_helperCellColors->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()][cellColors->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::PointNormalsMultiArrayType Mesh::getPointNormals() const
{
    ::fwData::Array::sptr pointNormals = m_mesh->getPointNormalsArray();
    return ::fwData::Mesh::PointNormalsMultiArrayType(
        static_cast< ::fwData::Mesh::PointNormalsMultiArrayType::element*>(m_helperPointNormals->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][pointNormals->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellNormalsMultiArrayType Mesh::getCellNormals() const
{
    ::fwData::Array::sptr cellNormals = m_mesh->getCellNormalsArray();
    return ::fwData::Mesh::CellNormalsMultiArrayType(
        static_cast< ::fwData::Mesh::CellNormalsMultiArrayType::element*>(m_helperCellNormals->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()][cellNormals->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::PointTexCoordsMultiArrayType Mesh::getPointTexCoords() const
{
    ::fwData::Array::sptr pointTexCoords = m_mesh->getPointTexCoordsArray();
    return ::fwData::Mesh::PointTexCoordsMultiArrayType(
        static_cast< ::fwData::Mesh::PointTexCoordsMultiArrayType::element*>(m_helperPointTexCoords->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfPoints()][pointTexCoords->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::CellTexCoordsMultiArrayType Mesh::getCellTexCoords() const
{
    ::fwData::Array::sptr cellTexCoords = m_mesh->getCellTexCoordsArray();
    return ::fwData::Mesh::CellTexCoordsMultiArrayType(
        static_cast< ::fwData::Mesh::CellTexCoordsMultiArrayType::element*>(m_helperCellTexCoords->getBuffer()),
        ::boost::extents[m_mesh->getNumberOfCells()][cellTexCoords->getNumberOfComponents()]
        );
}

//------------------------------------------------------------------------------

::fwData::Mesh::csptr Mesh::getMesh() const
{
    return m_mesh;
}

//------------------------------------------------------------------------------

bool Mesh::isClosed()
{
    bool isClosed = false;

    ::fwData::Mesh::Size cellDataSize = m_mesh->getCellDataSize();
    ::fwData::Mesh::Size nbOfCells    = m_mesh->getNumberOfCells();

    ::fwData::Mesh::CellId* cellDataBegin =
        m_helperCellData->begin< ::fwData::Mesh::CellId >();
    ::fwData::Mesh::CellId* cellDataEnd          = cellDataBegin + cellDataSize;
    ::fwData::Mesh::CellId* cellDataOffsetsBegin =
        m_helperCellDataOffsets->begin< ::fwData::Mesh::CellId >();
    ::fwData::Mesh::CellId* cellDataOffsetsEnd = cellDataOffsetsBegin + nbOfCells;
    ::fwData::Mesh::CellTypes* cellTypesBegin  = m_helperCellTypes->begin< ::fwData::Mesh::CellTypes >();

    isClosed = ::fwMath::isBorderlessSurface(cellDataBegin,
                                             cellDataEnd, cellDataOffsetsBegin,
                                             cellDataOffsetsEnd, cellTypesBegin );
    return isClosed;
}

//------------------------------------------------------------------------------

} // namespace helper

} // namespace fwDataTools

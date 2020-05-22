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

#pragma once

#include "fwDataTools/config.hpp"
#include "fwDataTools/helper/Array.hpp"

#include <fwData/Mesh.hpp>

#include <fwMemory/BufferObject.hpp>

#include <fwTools/Type.hpp>

namespace fwDataTools
{

namespace helper
{

/**
 * @brief   Helper to manage Mesh. Lock the mesh buffer before to modify it.
 * @deprecated The helper is no longer required to access the buffers. It will be removed in sight 22.0
 */
class FWDATATOOLS_CLASS_API Mesh
{
public:

    /// Shared pointer type
    typedef std::shared_ptr < ::fwDataTools::helper::Mesh > sptr;
    /// Const shared pointer type
    typedef std::shared_ptr < const ::fwDataTools::helper::Mesh > csptr;
    /// Unique pointer type
    typedef std::unique_ptr < ::fwDataTools::helper::Mesh > uptr;
    /// Const unique pointer type
    typedef std::unique_ptr < const ::fwDataTools::helper::Mesh > cuptr;

    //------------------------------------------------------------------------------

    static sptr New(::fwData::Mesh::sptr mesh)
    {
        return std::make_shared< ::fwDataTools::helper::Mesh >(mesh);
    }

    FWDATATOOLS_API Mesh( ::fwData::Mesh::sptr mesh );

    FWDATATOOLS_API virtual ~Mesh();

    /// Updates locks on all internal data Array of Mesh
    FWDATATOOLS_API void updateLock();

    /**
     * @brief Insert a point into the mesh.
     * Reallocates the point array if needed.
     *
     * @param p point coordinates
     *
     * @return The id of the new point
     *
     * @throw ::fwData::Exception
     */
    FWDATATOOLS_API  ::fwData::Mesh::Id insertNextPoint(const ::fwData::Mesh::PointValueType p[3]);

    /**
     * @brief Insert a point into the mesh.
     * @see insertNextPoint
     */
    FWDATATOOLS_API ::fwData::Mesh::Id insertNextPoint(::fwData::Mesh::PointValueType x,
                                                       ::fwData::Mesh::PointValueType y,
                                                       ::fwData::Mesh::PointValueType z);

    /**
     * @brief Set a point coordinates.
     * The mesh must be allocated before calling this method.
     *
     * @param id point index
     * @param p point coordinates
     *
     * @throw ::fwData::Exception
     */
    FWDATATOOLS_API void setPoint(::fwData::Mesh::Id id, const ::fwData::Mesh::PointValueType p[3]);

    /**
     * @brief Set a point coordinates.
     * @see setPoint
     */
    FWDATATOOLS_API void setPoint(::fwData::Mesh::Id id,
                                  ::fwData::Mesh::PointValueType x,
                                  ::fwData::Mesh::PointValueType y,
                                  ::fwData::Mesh::PointValueType z);

    /**
     * @brief Set a point color.
     *
     * Depending on the type of allocated point-colors array (RGB or RGBA), 3
     * or 4 points will be read from p.
     * The color array must be allocated before calling this method.
     *
     * @param id point index
     * @param c color
     *
     * @throw ::fwData::Exception
     */
    FWDATATOOLS_API void setPointColor(::fwData::Mesh::Id id, const ::fwData::Mesh::ColorValueType c[4]);

    /**
     * @brief Set a cell color.
     *
     * Depending on the type of allocated cell-colors array (RGB or RGBA), 3 or
     * 4 points will be read from p.
     * The color array must be allocated before calling this method.
     *
     * @param id cell index
     * @param c color
     *
     * @throw ::fwData::Exception
     */
    FWDATATOOLS_API void setCellColor(::fwData::Mesh::Id id, const ::fwData::Mesh::ColorValueType c[4]);

    /**
     * @brief Set a point normal.
     *
     * The normal array must be allocated before calling this method.
     *
     * @param id point index
     * @param n normal
     *
     * @throw ::fwData::Exception
     */
    FWDATATOOLS_API void setPointNormal(::fwData::Mesh::Id id, const ::fwData::Mesh::NormalValueType n[3]);

    /**
     * @brief Set a cell normal.
     *
     * The normal array must be allocated before calling this method.
     *
     * @param id cell index
     * @param n normal
     *
     * @throw ::fwData::Exception
     */
    FWDATATOOLS_API void setCellNormal(::fwData::Mesh::Id id, const ::fwData::Mesh::NormalValueType n[3]);

    /**
     * @brief Set a point texCoord.
     *
     * The texCoord array must be allocated before calling this method.
     *
     * @param id point index
     * @param t texCoord
     *
     * @throw ::fwData::Exception
     */
    FWDATATOOLS_API void setPointTexCoord(::fwData::Mesh::Id id, const ::fwData::Mesh::TexCoordValueType t[2]);

    /**
     * @brief Set a cell texCoord.
     *
     * The texCoord array must be allocated before calling this method.
     *
     * @param id cell index
     * @param t texCoord
     *
     * @throw ::fwData::Exception
     */
    FWDATATOOLS_API void setCellTexCoord(::fwData::Mesh::Id id, const ::fwData::Mesh::TexCoordValueType t[2]);

    /**
     * @brief Insert a cell into the mesh.
     *
     * Reallocates the mesh concerned arrays if needed.
     *
     * @return The id of the new cell
     *
     * @throw ::fwData::Exception
     */
    FWDATATOOLS_API ::fwData::Mesh::Id insertNextCell(::fwData::Mesh::CellTypesEnum type,
                                                      const ::fwData::Mesh::CellValueType* cell,
                                                      size_t nb );
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    FWDATATOOLS_API ::fwData::Mesh::Id insertNextCell(::fwData::Mesh::CellValueType p);
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    FWDATATOOLS_API ::fwData::Mesh::Id insertNextCell(::fwData::Mesh::CellValueType p1,
                                                      ::fwData::Mesh::CellValueType p2 );
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    FWDATATOOLS_API ::fwData::Mesh::Id insertNextCell(::fwData::Mesh::CellValueType p1,
                                                      ::fwData::Mesh::CellValueType p2,
                                                      ::fwData::Mesh::CellValueType p3 );
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    FWDATATOOLS_API ::fwData::Mesh::Id insertNextCell(::fwData::Mesh::CellValueType p1,
                                                      ::fwData::Mesh::CellValueType p2,
                                                      ::fwData::Mesh::CellValueType p3,
                                                      ::fwData::Mesh::CellValueType p4,
                                                      ::fwData::Mesh::CellTypesEnum type = ::fwData::Mesh::QUAD);
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    template <typename T> ::fwData::Mesh::Id insertNextCell(::fwData::Mesh::CellTypesEnum type,
                                                            const T& pointsBegin,
                                                            const T& pointsEnd );

    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::PointsMultiArrayType          getPoints() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::CellTypesMultiArrayType       getCellTypes() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::CellDataMultiArrayType        getCellData() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::CellDataOffsetsMultiArrayType getCellDataOffsets() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::PointColorsMultiArrayType     getPointColors() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::CellColorsMultiArrayType      getCellColors() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::PointNormalsMultiArrayType    getPointNormals() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::CellNormalsMultiArrayType     getCellNormals() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::PointTexCoordsMultiArrayType  getPointTexCoords() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    FWDATATOOLS_API ::fwData::Mesh::CellTexCoordsMultiArrayType   getCellTexCoords() const;

    /// Returns the internal ::fwData::Mesh
    FWDATATOOLS_API ::fwData::Mesh::csptr getMesh() const;

    /*!
     * @brief Return true if the mesh is closed.
     * The result is computed for each call.
     */
    [[deprecated("moved to ::fwDataTools::Mesh::isClosed()")]] FWDATATOOLS_API bool isClosed();

protected:

    ::fwData::Mesh::sptr m_mesh;

    ::fwDataTools::helper::Array::sptr m_helperPoints;
    ::fwDataTools::helper::Array::sptr m_helperCellTypes;
    ::fwDataTools::helper::Array::sptr m_helperCellData;
    ::fwDataTools::helper::Array::sptr m_helperCellDataOffsets;
    ::fwDataTools::helper::Array::sptr m_helperPointColors;
    ::fwDataTools::helper::Array::sptr m_helperCellColors;
    ::fwDataTools::helper::Array::sptr m_helperPointNormals;
    ::fwDataTools::helper::Array::sptr m_helperCellNormals;
    ::fwDataTools::helper::Array::sptr m_helperPointTexCoords;
    ::fwDataTools::helper::Array::sptr m_helperCellTexCoords;

};

} // namespace helper

} // namespace fwData

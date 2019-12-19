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
#include "fwVtkIO/vtk.hpp"

#include <fwData/Image.hpp>
#include <fwData/ObjectLock.hpp>

#include <fwDataTools/helper/Image.hpp>
#include <fwDataTools/helper/ImageGetter.hpp>

#include <fwMath/MeshFunctions.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/cast.hpp>

#include <vtkCell.h>
#include <vtkCellType.h>
#include <vtkDataArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkImageData.h>
#include <vtkImageExport.h>
#include <vtkImageImport.h>
#include <vtkLookupTable.h>
#include <vtkMatrix4x4.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataWriter.h>
#include <vtkSetGet.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>
#include <vtkUnstructuredGrid.h>

#include <cstring>
#include <functional>
#include <numeric>
#include <stdexcept>

namespace fwVtkIO
{

// ------------------------------------------------------------------------------

TypeTranslator::fwToolsToVtkMap::mapped_type TypeTranslator::translate(
    const TypeTranslator::fwToolsToVtkMap::key_type& key )
{
    fwToolsToVtkMap::const_iterator it = s_toVtk.find( key );
    FW_RAISE_IF("Unknown Type: " << key, it == s_toVtk.end() );
    return it->second;
}

// ------------------------------------------------------------------------------

TypeTranslator::VtkTofwToolsMap::mapped_type TypeTranslator::translate(
    const TypeTranslator::VtkTofwToolsMap::key_type& key )
{
    VtkTofwToolsMap::const_iterator it = s_fromVtk.find( key );
    FW_RAISE_IF("Unknown Type: " << key, it == s_fromVtk.end() );
    return it->second;
}

const TypeTranslator::fwToolsToVtkMap TypeTranslator::s_toVtk
    = boost::assign::map_list_of
      // char and signed char are treated as the same type.
      // and plain char is used when writing an int8 image
          ( fwTools::Type::create("int8" ), VTK_CHAR )
          ( fwTools::Type::create("uint8" ), VTK_UNSIGNED_CHAR )

          ( fwTools::Type::create("int16"), VTK_SHORT )
          ( fwTools::Type::create("uint16"), VTK_UNSIGNED_SHORT )

          ( fwTools::Type::create("int32"), VTK_INT )
          ( fwTools::Type::create("uint32"), VTK_UNSIGNED_INT )

          ( fwTools::Type::create("float" ), VTK_FLOAT )
          ( fwTools::Type::create("double"), VTK_DOUBLE )

#if ( INT_MAX < LONG_MAX )
      ( fwTools::Type::create("int64"), VTK_LONG )
          ( fwTools::Type::create("uint64"), VTK_UNSIGNED_LONG )
#endif
    ;

const TypeTranslator::VtkTofwToolsMap TypeTranslator::s_fromVtk
    = boost::assign::map_list_of

      // char and signed char are treated as the same type.
      // and plain char is used when writing an int8 image
          ( VTK_SIGNED_CHAR, fwTools::Type::create("int8" )  )
          ( VTK_CHAR, fwTools::Type::create("int8" )  )
          ( VTK_UNSIGNED_CHAR, fwTools::Type::create("uint8" ) )

          ( VTK_SHORT, fwTools::Type::create("int16")  )
          ( VTK_UNSIGNED_SHORT, fwTools::Type::create("uint16") )

          ( VTK_INT, fwTools::Type::create("int32")  )
          ( VTK_UNSIGNED_INT, fwTools::Type::create("uint32") )

          ( VTK_FLOAT, fwTools::Type::create("float" ) )
          ( VTK_DOUBLE, fwTools::Type::create("double") )

#if ( INT_MAX < LONG_MAX )
      ( VTK_LONG, fwTools::Type::create("int64")  )
          ( VTK_UNSIGNED_LONG, fwTools::Type::create("uint64") )

          ( VTK___INT64, fwTools::Type::create("int64")  )
          ( VTK_LONG_LONG, fwTools::Type::create("int64")  )

          ( VTK_UNSIGNED___INT64, fwTools::Type::create("uint64") )
          ( VTK_UNSIGNED_LONG_LONG, fwTools::Type::create("uint64") )
#else
      ( VTK_LONG, fwTools::Type::create("int32")  )
          ( VTK_UNSIGNED_LONG, fwTools::Type::create("uint32") )
#endif
    ;

// -----------------------------------------------------------------------------

void toVTKImage( ::fwData::Image::csptr data,  vtkImageData* dst)
{
    vtkSmartPointer< vtkImageImport > importer = vtkSmartPointer< vtkImageImport >::New();

    configureVTKImageImport( importer, data );

    importer->Update();

    dst->ShallowCopy(importer->GetOutput());
}

// -----------------------------------------------------------------------------

template< typename IMAGETYPE >
void* newBuffer(size_t size)
{
    IMAGETYPE* destBuffer;
    try
    {
        destBuffer = new IMAGETYPE[ size ];
    }
    catch (std::exception& e)
    {
        OSLM_ERROR("No enough memory to allocate an image of type "
                   << fwTools::makeDynamicType<IMAGETYPE>().string()
                   << " and of size "<< size << "." << std::endl
                   << e.what() );
        throw;
    }
    return destBuffer;
}

// -----------------------------------------------------------------------------

template< typename IMAGETYPE >
void fromRGBBuffer( void* input, size_t size, void*& destBuffer)
{
    if(destBuffer == NULL)
    {
        destBuffer = newBuffer<IMAGETYPE>(size);
    }

    IMAGETYPE* destBufferTyped = static_cast<IMAGETYPE*>(destBuffer);
    IMAGETYPE* inputTyped = static_cast<IMAGETYPE*>(input);
    IMAGETYPE* finalPtr = static_cast<IMAGETYPE*>(destBuffer) + size;
    IMAGETYPE valR, valG, valB;

    while (destBufferTyped < finalPtr)
    {
        valR                 = static_cast<IMAGETYPE>(float((*(inputTyped++)) * 0.30));
        valG                 = static_cast<IMAGETYPE>(float((*(inputTyped++)) * 0.59));
        valB                 = static_cast<IMAGETYPE>(float((*(inputTyped++)) * 0.11));
        (*destBufferTyped++) = valR + valG + valB;
    }
}

// -----------------------------------------------------------------------------

template< typename IMAGETYPE >
void fromRGBBufferColor( void* input, size_t size, void*& destBuffer)
{
    if(destBuffer == NULL)
    {
        destBuffer = newBuffer<IMAGETYPE>(size);
    }

    IMAGETYPE* destBufferTyped = static_cast<IMAGETYPE*>(destBuffer);
    IMAGETYPE* inputTyped      = static_cast<IMAGETYPE*>(input);
    IMAGETYPE* finalPtr        = static_cast<IMAGETYPE*>(destBuffer) + size;

    while (destBufferTyped < finalPtr)
    {
        (*destBufferTyped++) = (*(inputTyped++));
    }
}

// -----------------------------------------------------------------------------

void fromVTKImage( vtkImageData* source, ::fwData::Image::sptr destination )
{
    SLM_ASSERT("vtkImageData source and/or ::fwData::Image destination are not correct", destination && source );

    // ensure image size correct
//    source->UpdateInformation();
//    source->PropagateUpdateExtent();

    int dim = source->GetDataDimension();
    OSLM_TRACE("source->GetDataDimension() : " << dim);

    if(dim == 2)
    {
        const ::fwData::Image::Size size = {static_cast<size_t>(source->GetDimensions()[0]),
                                            static_cast<size_t>(source->GetDimensions()[1]), 0};
        destination->setSize2(size);

        const ::fwData::Image::Spacing spacing = {source->GetSpacing()[0], source->GetSpacing()[1], 0.};
        destination->setSpacing2(spacing);

        const ::fwData::Image::Origin origin = {source->GetOrigin()[0], source->GetOrigin()[1], 0.};
        destination->setOrigin2(origin);
    }
    else
    {
        const ::fwData::Image::Size size = {static_cast<size_t>(source->GetDimensions()[0]),
                                            static_cast<size_t>(source->GetDimensions()[1]),
                                            static_cast<size_t>(source->GetDimensions()[2])};
        destination->setSize2(size);

        const ::fwData::Image::Spacing spacing =
        {source->GetSpacing()[0], source->GetSpacing()[1], source->GetSpacing()[2]};
        destination->setSpacing2(spacing);

        const ::fwData::Image::Origin origin = {source->GetOrigin()[0], source->GetOrigin()[1], source->GetOrigin()[2]};
        destination->setOrigin2(origin);
    }

    const int nbComponents = source->GetNumberOfScalarComponents();
    const size_t size      = static_cast<size_t>(
        std::accumulate(source->GetDimensions(), source->GetDimensions()+dim, std::max(3,
                                                                                       nbComponents),
                        std::multiplies<size_t>() ));
    const void* input = source->GetScalarPointer();

    if (size != 0)
    {
        void* destBuffer;

        OSLM_TRACE("image size : " << size << " - nbBytePerPixel : " << source->GetScalarSize() );
        OSLM_TRACE(nbComponents << " components, " << TypeTranslator::translate( source->GetScalarType() ));
        destination->setType( TypeTranslator::translate( source->GetScalarType() ) );
        destination->setNumberOfComponents(static_cast<size_t>(nbComponents));
        if (nbComponents == 1)
        {
            destination->setPixelFormat(::fwData::Image::PixelFormat::GRAY_SCALE);
        }
        else if (nbComponents == 3)
        {
            destination->setPixelFormat(::fwData::Image::PixelFormat::RGB);
        }
        else if (nbComponents == 4)
        {
            destination->setPixelFormat(::fwData::Image::PixelFormat::RGBA);
        }
        destination->resize();

        const auto dumpLock = destination->lock();

        destBuffer = destination->getBuffer();
        const size_t sizeInBytes = destination->getSizeInBytes();
        std::memcpy(destBuffer, input, sizeInBytes);
    }
}

// ------------------------------------------------------------------------------

void configureVTKImageImport( ::vtkImageImport* _pImageImport, ::fwData::Image::csptr _pDataImage )
{
    const auto dumpLock = _pDataImage->lock();

    if(_pDataImage->getNumberOfDimensions() == 2)
    {
        _pImageImport->SetDataSpacing(  _pDataImage->getSpacing2()[0],
                                        _pDataImage->getSpacing2()[1],
                                        0
                                        );

        _pImageImport->SetDataOrigin(   _pDataImage->getOrigin2()[0],
                                        _pDataImage->getOrigin2()[1],
                                        0
                                        );

        _pImageImport->SetWholeExtent(  0, static_cast<int>(_pDataImage->getSize2()[0]) - 1,
                                        0, static_cast<int>(_pDataImage->getSize2()[1]) - 1,
                                        0, 0
                                        );
    }
    else
    {
        _pImageImport->SetDataSpacing(  _pDataImage->getSpacing2()[0],
                                        _pDataImage->getSpacing2()[1],
                                        _pDataImage->getSpacing2()[2]
                                        );

        _pImageImport->SetDataOrigin(   _pDataImage->getOrigin2()[0],
                                        _pDataImage->getOrigin2()[1],
                                        _pDataImage->getOrigin2()[2]
                                        );

        _pImageImport->SetWholeExtent(  0, static_cast<int>(_pDataImage->getSize2()[0]) - 1,
                                        0, static_cast<int>(_pDataImage->getSize2()[1]) - 1,
                                        0, static_cast<int>(_pDataImage->getSize2()[2]) - 1
                                        );
    }

    _pImageImport->SetNumberOfScalarComponents(static_cast<int>( _pDataImage->getNumberOfComponents() ));

    // copy WholeExtent to DataExtent
    _pImageImport->SetDataExtentToWholeExtent();
    // no copy, no buffer destruction/management
    _pImageImport->SetImportVoidPointer( _pDataImage->getBuffer() );
    // used to set correct pixeltype to VtkImage
    _pImageImport->SetDataScalarType( TypeTranslator::translate(_pDataImage->getType()) );
}

// -----------------------------------------------------------------------------

vtkSmartPointer<vtkMatrix4x4> toVTKMatrix( ::fwData::TransformationMatrix3D::csptr _transfoMatrix )
{
    auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();
    for(std::uint8_t l = 0; l < 4; l++)
    {
        for(std::uint8_t c = 0; c < 4; c++)
        {
            matrix->SetElement(l, c, _transfoMatrix->getCoefficient(l, c));
        }
    }
    return matrix;
}

// -----------------------------------------------------------------------------

bool fromVTKMatrix( vtkMatrix4x4* _matrix, ::fwData::TransformationMatrix3D::sptr _transfoMatrix)
{
    bool res = true;
    for(std::uint8_t l = 0; l < 4; l++)
    {
        for(std::uint8_t c = 0; c < 4; c++)
        {
            _transfoMatrix->setCoefficient(l, c, _matrix->GetElement(l, c));
        }
    }
    return res;
}

// -----------------------------------------------------------------------------

} // namespace fwVtkIO

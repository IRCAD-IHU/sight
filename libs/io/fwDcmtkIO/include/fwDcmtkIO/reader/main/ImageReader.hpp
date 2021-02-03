/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwDcmtkIO/config.hpp"

#include <data/DicomSeries.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

namespace fwDcmtkIO
{
namespace reader
{
namespace main
{

/**
 * @brief This class is used to read the buffer of a DICOM image in DIRECT mode.
 */
class FWDCMTKIO_CLASS_API ImageReader
{
public:
    typedef data::DicomSeries::DicomContainerType DicomContainerType;

    /**
     * @brief Fill the buffer of an image
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] depth Number of frames
     * @param[in] instances Paths to the instances
     * @param[in] destination Destination buffer
     * @param[in] rescaleSlope Slope parameter
     * @param[in] rescaleIntercept Intercept parameter
     * @param[in] pixelRepresentation Pixel representation of the image (signed or unsigned short)
     * @param[in] imageType Type of the image used to create the buffer
     */
    static void fillImageBuffer(unsigned int rows, unsigned int columns,
                                unsigned int depth,
                                DicomContainerType& instances,
                                void* destination, double rescaleSlope,
                                double rescaleIntercept,
                                unsigned short pixelRepresentation, core::tools::Type imageType)
    {
        if (pixelRepresentation == 0)
        {
            // Use unsigned char
            if(imageType.sizeOf() == 1)
            {
                ::fwDcmtkIO::reader::main::ImageReader::fillImageBuffer< Uint8 >(
                    rows, columns, depth, instances, destination, rescaleSlope, rescaleIntercept, imageType);
            }
            // Use unsigned short
            else
            {
                ::fwDcmtkIO::reader::main::ImageReader::fillImageBuffer< Uint16 >(
                    rows, columns, depth, instances, destination, rescaleSlope, rescaleIntercept, imageType);
            }
        }
        else
        {
            // Use signed char
            if(imageType.sizeOf() == 1)
            {
                ::fwDcmtkIO::reader::main::ImageReader::fillImageBuffer< Sint8 >(
                    rows, columns, depth, instances, destination, rescaleSlope, rescaleIntercept, imageType);
            }
            // Use signed short
            else
            {
                ::fwDcmtkIO::reader::main::ImageReader::fillImageBuffer< Sint16 >(
                    rows, columns, depth, instances, destination, rescaleSlope, rescaleIntercept, imageType);
            }
        }
    }

    /**
     * @brief Create the signed short or unsigned short temporary buffer used to fill the image buffer
     * The template T is used to determine if we must use signed or unsigned short to create the buffer
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] depth Number of frames
     * @param[in] instances Paths to the instances
     */
    template< typename T >
    static T* createTemporaryBuffer(unsigned int rows, unsigned int columns,
                                    unsigned int depth,
                                    DicomContainerType& instances)
    {
        OFCondition status;
        DcmDataset* dataset;

        T* tempoBuffer     = new T[rows*columns*depth];
        uint32_t sliceSize = rows*columns;

        uint32_t copySize;
        if(instances.size() == 1 )
        {
            copySize = rows*columns*depth;
        }
        else
        {
            copySize = rows*columns;
        }

        // Slice index
        unsigned short z = 0;

        // Read every instances
        for(const auto& item : instances)
        {
            const core::memory::BufferObject::sptr bufferObj = item.second;
            const size_t buffSize                            = bufferObj->getSize();
            const std::string dicomPath                      = bufferObj->getStreamInfo().fsFile.string();
            core::memory::BufferObject::Lock lock(bufferObj);
            char* buffer = static_cast< char* >( lock.getBuffer() );

            DcmInputBufferStream is;
            is.setBuffer(buffer, offile_off_t(buffSize));
            is.setEos();

            DcmFileFormat fileFormat;
            fileFormat.transferInit();
            if (!fileFormat.read(is).good())
            {
                FW_RAISE("Unable to read Dicom file '"<< dicomPath <<"' "<<
                         "(slice: '" << item.first << "')");
            }

            fileFormat.loadAllDataIntoMemory();
            fileFormat.transferEnd();

            dataset = fileFormat.getDataset();

            // Decompress data set if compressed
            dataset->chooseRepresentation(EXS_LittleEndianExplicit, nullptr);

            const Uint16* pixelData;
            dataset->findAndGetUint16Array(DCM_PixelData, pixelData);

            if(pixelData)
            {
                //Add the slice to the temporary buffer
                memcpy(tempoBuffer+z*sliceSize, pixelData, copySize*sizeof(T));
                ++z;
            }
            else
            {
                SLM_WARN("There is no pixel data in file \"" + dicomPath + "\".");
            }

        }

        return tempoBuffer;
    }

protected:

    /**
     * @brief Fill the buffer of an image
     * The template T is used to determine if we must use signed or unsigned short when reading pixels.
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] depth Number of frames
     * @param[in] instances Paths to the instances
     * @param[in] destination Destination buffer
     * @param[in] rescaleSlope Slope parameter
     * @param[in] rescaleIntercept Intercept parameter
     * @param[in] imageType Type of the image used to create the buffer
     */
    template< typename T >
    static void fillImageBuffer(unsigned int rows, unsigned int columns,
                                unsigned int depth,
                                DicomContainerType& instances,
                                void* destination, double rescaleSlope,
                                double rescaleIntercept,
                                core::tools::Type imageType)
    {
        T* tempoBuffer = ::fwDcmtkIO::reader::main::ImageReader::createTemporaryBuffer<T>(
            rows, columns, depth, instances);

        //Copy the temporary buffer to the final buffer
        if (imageType == core::tools::Type::s_INT8)
        {
            ::fwDcmtkIO::reader::main::ImageReader::copyBuffer< std::int8_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == core::tools::Type::s_INT16)
        {
            ::fwDcmtkIO::reader::main::ImageReader::copyBuffer< std::int16_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == core::tools::Type::s_INT32)
        {
            ::fwDcmtkIO::reader::main::ImageReader::copyBuffer< std::int32_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == core::tools::Type::s_INT64)
        {
            ::fwDcmtkIO::reader::main::ImageReader::copyBuffer< std::int64_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == core::tools::Type::s_UINT8)
        {
            ::fwDcmtkIO::reader::main::ImageReader::copyBuffer< std::uint8_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == core::tools::Type::s_UINT16)
        {
            ::fwDcmtkIO::reader::main::ImageReader::copyBuffer< std::uint16_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == core::tools::Type::s_UINT32)
        {
            ::fwDcmtkIO::reader::main::ImageReader::copyBuffer< std::uint32_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == core::tools::Type::s_UINT64)
        {
            ::fwDcmtkIO::reader::main::ImageReader::copyBuffer< std::uint64_t >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == core::tools::Type::s_FLOAT)
        {
            ::fwDcmtkIO::reader::main::ImageReader::copyBuffer< float >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }
        else if (imageType == core::tools::Type::s_DOUBLE)
        {
            ::fwDcmtkIO::reader::main::ImageReader::copyBuffer< double >(
                rows, columns, depth, tempoBuffer, destination, rescaleSlope,
                rescaleIntercept);
        }

        delete tempoBuffer;
    }

    /**
     * @brief Copy the temporary buffer to the image buffer
     * The template T is used to determine the type of the final image buffer
     * The template U is used to determine the type of the temporary buffer
     * @param[in] rows Number of rows
     * @param[in] columns Number of columns
     * @param[in] depth Number of frames
     * @param[in] source Temporary buffer
     * @param[in] destination Final image buffer
     * @param[in] rescaleSlope Slope parameter
     * @param[in] rescaleIntercept Intercept parameter
     */
    template< typename T, typename U >
    static void copyBuffer(unsigned int rows, unsigned int columns,
                           unsigned int depth,
                           U* source,
                           void* destination,
                           double rescaleSlope, double rescaleIntercept)
    {
        T* arrayBuffer = static_cast< T* >(destination);
        unsigned short x, y, z;
        unsigned int frameSize = columns*rows;
        x = y = z = 0;
        for(x = 0; x < columns; ++x)
        {
            for(y = 0; y < rows; ++y)
            {
                unsigned int yshift = x*rows;

                for(z = 0; z < depth; ++z)
                {
                    unsigned int position = y + yshift + z*frameSize;
                    T value               = (T)source[position];
                    value                 = static_cast< T >(rescaleSlope * value + rescaleIntercept);
                    arrayBuffer[position] = value;
                }
            }
        }
    }

};

} //main
} //reader
} //fwDcmtkIO

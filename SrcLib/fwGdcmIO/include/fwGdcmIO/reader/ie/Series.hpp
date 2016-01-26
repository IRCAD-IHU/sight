/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_IE_SERIES_HPP__
#define __FWGDCMIO_READER_IE_SERIES_HPP__

#include "fwGdcmIO/reader/ie/InformationEntity.hpp"

#include <fwMedData/Series.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @class Series
 * @brief Series Information Entity class
 */
class FWGDCMIO_CLASS_API Series : public ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::Series >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] series Series data
     * @param[in] logger Logger
     * @param[in] callback Progress callback
     * @param[in] cancelled cancel information
     */
    FWGDCMIO_API Series(SPTR(::fwDicomData::DicomSeries)dicomSeries,
                        SPTR(::gdcm::Reader)reader,
                        SPTR(::fwGdcmIO::container::DicomInstance)instance,
                        ::fwMedData::Series::sptr series,
                        ::fwLog::Logger::sptr logger,
                        const ProgressCallback& callback,
                        const bool& cancelled);

    /// Destructor
    FWGDCMIO_API virtual ~Series();

    /**
     * @brief Read General Series Module tags
     * @see PS 3.3 C.7.3.1
     */
    FWGDCMIO_API virtual void readGeneralSeriesModule();

};

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO

#endif /* __FWGDCMIO_READER_IE_SERIES_HPP__ */

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_IE_PATIENT_HPP__
#define __FWGDCMIO_READER_IE_PATIENT_HPP__

#include "fwGdcmIO/reader/ie/InformationEntity.hpp"

#include <fwMedData/Patient.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @class Patient
 * @brief Patient Information Entity class
 */
class FWGDCMIO_CLASS_API Patient : public ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::Patient >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read patient.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] patient Patient data
     * @param[in] logger Logger
     * @param[in] callback Progress callback
     * @param[in] cancelled cancel information
     */
    FWGDCMIO_API Patient(SPTR(::fwDicomData::DicomSeries)dicomSeries,
                         SPTR(::gdcm::Reader)reader,
                         SPTR(::fwGdcmIO::container::DicomInstance)instance,
                         ::fwMedData::Patient::sptr patient,
                         ::fwLog::Logger::sptr logger,
                         const ProgressCallback& callback,
                         const bool& cancelled);

    /// Destructor
    FWGDCMIO_API virtual ~Patient();

    /**
     * @brief Read Patient Module tags
     * @see PS 3.3 C.7.1.1
     */
    FWGDCMIO_API virtual void readPatientModule();
};

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO

#endif // __FWGDCMIO_READER_IE_PATIENT_HPP__

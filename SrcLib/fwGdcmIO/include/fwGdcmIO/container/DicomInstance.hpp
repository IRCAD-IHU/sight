/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_CONTAINER_DICOMINSTANCE_HPP__
#define __FWGDCMIO_CONTAINER_DICOMINSTANCE_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwCore/macros.hpp>

#include <gdcmMediaStorage.h>

#include <string>
#include <vector>

namespace fwDicomData
{
class DicomSeries;
}

namespace fwMedData
{
class Series;
}

namespace fwGdcmIO
{
namespace container
{

/**
 * @class DicomInstance
 * @brief This class defines a DICOM SOP instance. It is useful during the whole writing process.
 * This class allows to share data between module writers.
 */
class FWGDCMIO_CLASS_API DicomInstance
{
public:

    typedef std::vector< std::string > SOPInstanceUIDContainerType;

    /// Constructor
    FWGDCMIO_API DicomInstance();

    /**
     * @brief Constructor
     * @param[in] series Series from which the instance is created
     * @param[in] isMultiFiles Set whether the instance must be split in several files or not
     */
    FWGDCMIO_API DicomInstance(SPTR(::fwMedData::Series)series, bool isMultiFiles);

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries from which the instance is created
     */
    FWGDCMIO_API DicomInstance(SPTR(::fwDicomData::DicomSeries)dicomSeries);

    /// Copy constructor
    FWGDCMIO_API DicomInstance(const DicomInstance & dicomInstance);

    /// Destructor
    FWGDCMIO_API virtual ~DicomInstance();

    /// Get the flag on multi-files state of an image series
    bool getIsMultiFiles() const
    {
        return m_isMultiFiles;
    }

    /// Set the flag on multi-files state of an image series
    void setIsMultiFiles(bool isMultiFiles)
    {
        m_isMultiFiles = isMultiFiles;
    }

    /// Get SOP Class UID
    const std::string &getSOPClassUID() const
    {
        return m_SOPClassUID;
    }

    /// Set SOP Class UID
    void setSOPClassUID(const std::string & SOPClassUID)
    {
        m_SOPClassUID = SOPClassUID;
    }

    /// Get Series Instance UID
    const std::string &getSeriesInstanceUID() const
    {
        return m_seriesInstanceUID;
    }

    /// Set Series Instance UID
    void setSeriesInstanceUID(const std::string & seriesInstanceUID)
    {
        m_seriesInstanceUID = seriesInstanceUID;
    }

    /// Get Study Instance UID
    const std::string &getStudyInstanceUID() const
    {
        return m_studyInstanceUID;
    }

    /// Set Study Instance UID
    void setStudyInstanceUID(const std::string & studyInstanceUID)
    {
        m_studyInstanceUID = studyInstanceUID;
    }

    /// Get SOP Instance UID container
    const SOPInstanceUIDContainerType& getSOPInstanceUIDContainer() const
    {
        return m_SOPInstanceUIDContainer;
    }

    /// Get SOP Instance UID container
    SOPInstanceUIDContainerType& getRefSOPInstanceUIDContainer()
    {
        return m_SOPInstanceUIDContainer;
    }

    /// Set SOP Instance UID Container
    void setSOPInstanceUIDContainer(const SOPInstanceUIDContainerType & SOPInstanceUIDContainer)
    {
        m_SOPInstanceUIDContainer = SOPInstanceUIDContainer;
    }

protected:
    /**
     * @brief Compute SOPClassUID
     * @param[in] series Series
     */
    void computeSOPClassUID(SPTR(::fwMedData::Series) series);

    /**
     * @brief Generate SOPInstanceUIDs according to series type and dimension
     * @param[in] series Series
     */
    void generateSOPInstanceUIDs(SPTR(::fwMedData::Series) series);

private:

    /// Define if the instance must be split in several files
    bool m_isMultiFiles;

    /// SOP Class UID
    std::string m_SOPClassUID;

    /// Study Instance UID
    std::string m_studyInstanceUID;

    /// Series Instance UID
    std::string m_seriesInstanceUID;

    /// SOP Instance UID container
    SOPInstanceUIDContainerType m_SOPInstanceUIDContainer;

};

}
//namespace container
}//namespace fwGdcmIO

#endif /* __FWGDCMIO_CONTAINER_DICOMINSTANCE_HPP__ */

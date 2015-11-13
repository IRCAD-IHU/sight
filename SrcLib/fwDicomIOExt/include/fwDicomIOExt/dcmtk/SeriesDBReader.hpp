/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_SERIESDBREADER_HPP__
#define __FWDICOMIOEXT_DCMTK_SERIESDBREADER_HPP__

#include "fwDicomIOExt/dcmtk/reader/IObjectReader.hpp"
#include "fwDicomIOExt/config.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwServices/IService.hpp>

#include <fwTools/ProgressAdviser.hpp>

namespace fwMedData
{
class SeriesDB;
class Patient;
class Study;
class Equipment;
class Series;
}

class DcmDataset;


namespace fwDicomIOExt
{
namespace dcmtk
{

/**
 * @class SeriesDBReader
 *
 * @brief Reads DICOM data from a directory path in order to create a SeriesDB object.
 */
class FWDICOMIOEXT_CLASS_API SeriesDBReader : public ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >,
                                              public ::fwData::location::enableFolder< ::fwDataIO::reader::IObjectReader >,
                                              public ::fwData::location::enableMultiFiles< ::fwDataIO::reader::
                                                                                           IObjectReader >,
                                              public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (SeriesDBReader)( ::fwDataIO::reader::GenericObjectReader< ::fwMedData::SeriesDB >),
        (()),
        ::fwDataIO::reader::factory::New< SeriesDBReader >
        );
    fwCoreAllowSharedFromThis();


    typedef std::map< std::string, SPTR(::fwMedData::Patient) > PatientMapType;
    typedef std::map< std::string, SPTR(::fwMedData::Study) > StudyMapType;
    typedef std::map< std::string, SPTR(::fwMedData::Equipment) > EquipmentMapType;
    typedef std::vector< SPTR(::fwDicomData::DicomSeries) > DicomSeriesContainerType;
    typedef std::vector< std::string > FilenameContainerType;

    FWDICOMIOEXT_API SeriesDBReader(::fwDataIO::reader::IObjectReader::Key key);

    FWDICOMIOEXT_API ~SeriesDBReader();

    /// Reads DICOM data from configured path and fills SeriesDB object
    FWDICOMIOEXT_API void read();

    /**
     * @brief Reads DICOM data from DicomSeries an fills SeriesDB object
     * @param[in] dicomSeriesDB SeriesDB containing DicomSeries that must be read
     */
    FWDICOMIOEXT_API void readFromDicomSeriesDB(::fwMedData::SeriesDB::sptr dicomSeriesDB);

    /// Reads DICOM data from configured path and fills SeriesDB object with DicomSeries
    FWDICOMIOEXT_API void readDicomSeries();

    /// Return true if a dicomdir file can be read.
    FWDICOMIOEXT_API bool isDicomDirAvailable();

    /// Return DicomSeries container
    FWDICOMIOEXT_API DicomSeriesContainerType &getDicomSeries();

    /**
     * @brief Set whether the reader must use the dicomdir file or not
     * @{ */
    const bool  getDicomdirActivated () const
    {
        return m_isDicomdirActivated;
    }
    bool & getRefDicomdirActivated ()
    {
        return this->m_isDicomdirActivated;
    }
    const bool &getCRefDicomdirActivated () const
    {
        return this->m_isDicomdirActivated;
    }
    void setDicomdirActivated (const bool _isDicomdirActivated)
    {
        this->m_isDicomdirActivated = _isDicomdirActivated;
    }
    /**  @} */

    /**
     * @brief Dicom filter type that must be applied prior the reading process
     * @{ */
    const std::string  getDicomFilterType () const
    {
        return m_dicomFilterType;
    }
    std::string & getRefDicomFilterType ()
    {
        return this->m_dicomFilterType;
    }
    const std::string &getCRefDicomFilterType () const
    {
        return this->m_dicomFilterType;
    }
    void setDicomFilterType (const std::string&  _dicomFilterType)
    {
        this->m_dicomFilterType = _dicomFilterType;
    }
    /**  @} */

private:

    /// Return filenames that must be read
    FilenameContainerType getFilenames();

    /**
     * @brief Fills the related SeriesDB object with DICOM data extracted from the given files.
     *
     * @param[in] seriesDB SeriesDB object to be filled with DICOM data
     * @param[in] filenames files to extract DICOM data from
     */
    void addSeries(const std::vector< std::string > &filenames);

    /**
     * @brief Create a patient from the dataset and store it in the patient map
     * @param[in] dataset Dataset from which the patient is created
     */
    SPTR(::fwMedData::Patient) createPatient(DcmDataset* dataset);

    /**
     * @brief Create a study from the dataset and store it in the study map
     * @param[in] dataset Dataset from which the study is created
     */
    SPTR(::fwMedData::Study) createStudy(DcmDataset* dataset);

    /**
     * @brief Create an equipment from the dataset and store it in the equipment map
     * @param[in] dataset Dataset from which the equipment is created
     */
    SPTR(::fwMedData::Equipment) createEquipment(DcmDataset* dataset);

    /**
     * @brief Create a series from the dataset and store it in the series map
     * @param[in] dataset Dataset from which the series is created
     * @param[in] filename Instance file
     */
    void createSeries(DcmDataset* dataset, const std::string& filename);

    /**
     * @brief Convert DicomSeries to Image or Model Series
     * @param[in] dicomSeries Dicom Series that must be converted
     * @param[in] notify if true, notify the seriesDB
     */
    void convertDicomSeries(SPTR(::fwDicomData::DicomSeries) dicomSeries,
                            bool notify = false);

    ///Patient Map
    PatientMapType m_patientMap;

    ///Study Map
    StudyMapType m_studyMap;

    ///Equipment Map
    EquipmentMapType m_equipmentMap;

    ///Object Reader Map
    DicomSeriesContainerType m_dicomSeriesContainer;

    ///True if the reader can use the dicomdir file.
    bool m_isDicomdirActivated;

    ///Dicom filter type that must be applied prior the reading process
    std::string m_dicomFilterType;

};

} //dcmtk
} // namespace fwDicomIOExt

#endif /*__FWDICOMIOEXT_DCMTK_SERIESDBREADER_HPP__*/

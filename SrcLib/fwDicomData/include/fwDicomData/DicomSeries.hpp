/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMDATA_DICOMSERIES_HPP__
#define __FWDICOMDATA_DICOMSERIES_HPP__

#include "fwMedData/types.hpp"
#include "fwMedData/Series.hpp"
#include "fwDicomData/config.hpp"

#include <fwData/Object.hpp>
#include <fwData/factory/new.hpp>

#include <boost/filesystem/path.hpp>

fwCampAutoDeclareDataMacro((fwDicomData)(DicomSeries), FWDICOMDATA_API);

namespace fwData
{
class Array;
}

namespace fwDicomData
{


/**
 * @brief Holds DICOM data
 */
class FWDICOMDATA_CLASS_API DicomSeries : public ::fwMedData::Series
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (DicomSeries)(::fwData::Object), (()),
                                            ::fwData::factory::New< DicomSeries >);

    fwCampMakeFriendDataMacro((fwDicomData)(DicomSeries));

    typedef std::map < std::size_t, ::boost::filesystem::path > DicomPathContainerType;

    typedef std::map < std::string, SPTR(::fwData::Array) > DicomBinaryContainerType;

    typedef std::set < std::string > SOPClassUIDContainerType;

    typedef std::map < std::string, std::string > ComputedTagValueContainerType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDICOMDATA_API DicomSeries(::fwData::Object::Key key);

    /// Destructor
    FWDICOMDATA_API virtual ~DicomSeries();

    /// Defines shallow copy
    FWDICOMDATA_API void shallowCopy( const ::fwData::Object::csptr &_source );

    /// Defines deep copy
    FWDICOMDATA_API void cachedDeepCopy( const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache );

    /// Add dicom path
    FWDICOMDATA_API void addDicomPath(std::size_t instanceIndex, ::boost::filesystem::path path);

    /// Add binary array
    FWDICOMDATA_API void addBinary(const std::string &filename, SPTR(::fwData::Array) binary);

    /**
     * @brief Return true if the instance is available on the local computer
     * @todo This method could be constifiable but function std::map::at doesn't work on windows
     */
    FWDICOMDATA_API bool isInstanceAvailable(std::size_t instanceIndex);

    /**
     * @brief Add a SOPClassUID that is used by this series.
     * @param[in] sopClassUID SOPClassUID that must be added.
     */
    FWDICOMDATA_API void addSOPClassUID(const std::string& sopClassUID);

    /**
     * @brief Add a computed value to the specified tag.
     * @param[in] tagName Name of the tag
     * @param[in] value Computed value
     */
    FWDICOMDATA_API void addComputedTagValue(const std::string& tagName, const std::string& value);

    /**
     * @brief Return true if there is a computed value for the specified tag
     * @param[in] tagName Name of the tag
     * @return True if there is a computed value for the tag
     */
    FWDICOMDATA_API bool hasComputedValues(const std::string& tagName) const;

    /**
     * @brief Availability
     */
    typedef enum
    {
        NONE     = 1,   /*! The DICOM files are not available on the local machine but may be available on the pacs. */
        PATHS    = 2,   /*! The paths to the DICOM files are saved in this DicomSeries. */
        BINARIES = 3,    /*! The binaries of the DICOM files are saved in this DicomSeries. */
        BLOB     = 4    /*! A set of paths that has not yet been read. Those paths may not be DICOM files. */
    } DICOM_AVAILABILITY;

    /**
     * @name Getters / Setters
     * @{ */

    /**
     * @brief Dicom files availability
     * @{ */
    DICOM_AVAILABILITY  getDicomAvailability () const
    {
        return m_dicomAvailability;
    }

    void setDicomAvailability(DICOM_AVAILABILITY val)
    {
        m_dicomAvailability = val;
    }
    /**  @} */

    /**
     * @brief Local paths of Dicom files
     * @{ */
    const DicomPathContainerType &getLocalDicomPaths() const
    {
        return m_localDicomPaths;
    }
    void setLocalDicomPaths(const DicomPathContainerType &val)
    {
        m_localDicomPaths = val;
    }
    /**  @} */

    /**
     * @brief Number of instances in the series (0020,1009)
     * @{ */
    size_t  getNumberOfInstances () const
    {
        return m_numberOfInstances;
    }
    void setNumberOfInstances (std::size_t val)
    {
        m_numberOfInstances = val;
    }
    /**  @} */

    /**
     * @brief Dicom binaries
     * @{ */
    const DicomBinaryContainerType &getDicomBinaries () const
    {
        return m_dicomBinaries;
    }
    void setDicomBinaries (const DicomBinaryContainerType &val)
    {
        m_dicomBinaries = val;
    }
    /**  @} */

    /**
     * @brief SOP Class UID
     * @{ */
    const SOPClassUIDContainerType &getSOPClassUIDs () const
    {
        return m_SOPClassUIDs;
    }
    void setSOPClassUIDs (const SOPClassUIDContainerType &val)
    {
        m_SOPClassUIDs = val;
    }
    /**  @} */

    /**
     * @brief Computed Tag Values
     * @{ */
    const ComputedTagValueContainerType &getComputedTagValues () const
    {
        return m_computedTagValues;
    }
    void setComputedTagValues (const ComputedTagValueContainerType &val)
    {
        m_computedTagValues = val;
    }
    /**  @} */

    /**
     * @brief First Instance Number (0 or 1) - Used for PACS preview
     * @{ */
    std::size_t getFirstInstanceNumber() const
    {
        return m_firstInstanceNumber;
    }

    void setFirstInstanceNumber(std::size_t firstInstanceNumber)
    {
        m_firstInstanceNumber = firstInstanceNumber;
    }
    /**  @} */


    /**  @} */


protected:

    /// Dicom Availability
    DICOM_AVAILABILITY m_dicomAvailability;

    /// Number of instances in the series (0020,1209)
    size_t m_numberOfInstances;

    /// Local paths of Dicom files
    DicomPathContainerType m_localDicomPaths;

    /// Dicom binaries
    DicomBinaryContainerType m_dicomBinaries;

    /// SOP Class UIDs
    SOPClassUIDContainerType m_SOPClassUIDs;

    /// Computed tag values
    ComputedTagValueContainerType m_computedTagValues;

    /// First instance number (0 or 1) - Used for PACS preview
    std::size_t m_firstInstanceNumber;
};

}   //end namespace fwDicomData

#endif // __FWDICOMDATA_DICOMSERIES_HPP__



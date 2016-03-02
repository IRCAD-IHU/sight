/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODCMTK_SSERIESDBREADER_HPP__
#define __IODCMTK_SSERIESDBREADER_HPP__

#include "ioDcmtk/config.hpp"

#include <io/IReader.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwMedData
{
class SeriesDB;
class Patient;
}

namespace ioDcmtk
{

/**
 * @brief  Read ImageSeries from DICOM with dcmtk
 **/
class IODCMTK_CLASS_API SSeriesDBReader : public ::io::IReader
{

public:
    fwCoreServiceClassDefinitionsMacro ( (SSeriesDBReader)( ::io::IReader) );
    /**
     * @brief   constructor
     *
     */
    IODCMTK_API SSeriesDBReader() throw();

    /**
     * @brief   destructor
     */
    IODCMTK_API virtual ~SSeriesDBReader() throw();

protected:

    typedef std::vector< std::string > SupportedSOPClassContainerType;

    /**
     * The reader can be configured as a regular reader.
     * You can select the SOP Classes that can be read using this reader.
     * It is also possible to define a filter that must be applied prior the reading process.
     * @code{.xml}
       <config filterType="::fwDicomIOFilter::custom::NoFilter" />
       <SOPClassSelection>
           <SOPClass name="CT Image Storage" uid="1.2.840.10008.5.1.4.1.1.2" />
       </SOPClassSelection>
       @endcode
     * On the other hand, you can specify a service configuration using the FilterSelectorSrvConfig element:
     * @code{.xml}
        <extension implements="::fwServices::registry::ServiceConfig">
            <id>FilterSelectorConfig</id>
            <service>::ioDicom::SFilterSelectorDialog</service>
            <desc>"Open" action's filter selector config</desc>
            <config>
                <selection mode="include" />
                <addSelection filter="::fwDicomIOFilter::custom::DefaultDicomFilter" />
                <addSelection filter="::fwDicomIOFilter::custom::NoFilter" />
            </config>
        </extension>
       @endcode
     */
    IODCMTK_API virtual void configuring() throw (fwTools::Failed);

    /// Override
    IODCMTK_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODCMTK_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODCMTK_API void updating() throw(::fwTools::Failed);

    /// Override
    IODCMTK_API void info(std::ostream &_sstream );

    /// Override
    IODCMTK_API virtual std::string getSelectorDialogTitle();

    /// Override
    IODCMTK_API virtual void configureWithIHM();

    /// Return path type managed by the service, here FOLDER
    IODCMTK_API ::io::IOPathType getIOPathType() const;

private:

    void notificationOfDBUpdate();

    SPTR(::fwMedData::SeriesDB) createSeriesDB(const ::boost::filesystem::path& dicomDir);

    /// Selector config used to select a filter to apply
    std::string m_filterSelectorSrvConfig;

    /// Selected filter key
    std::string m_filterType;

    /// Supported SOP Class selection
    SupportedSOPClassContainerType m_supportedSOPClassSelection;

};

} // namespace ioDcmtk

#endif //__IODCMTK_SSERIESDBREADER_HPP__

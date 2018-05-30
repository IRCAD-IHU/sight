/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilter/splitter/SOPClassUIDSplitter.hpp"

#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/registry/macros.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmb.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmnet/diutil.h>

fwDicomIOFilterRegisterMacro( ::fwDicomIOFilter::splitter::SOPClassUIDSplitter );

namespace fwDicomIOFilter
{
namespace splitter
{

const std::string SOPClassUIDSplitter::s_FILTER_NAME        = "SOPClassUID splitter";
const std::string SOPClassUIDSplitter::s_FILTER_DESCRIPTION =
    "Split instances according to <i>SOPClassUID</i> tag.";

//-----------------------------------------------------------------------------

SOPClassUIDSplitter::SOPClassUIDSplitter(::fwDicomIOFilter::IFilter::Key key) :
    ::fwDicomIOFilter::splitter::TagValueSplitter(key)
{
    this->setTag(DCM_SOPClassUID);
}

//-----------------------------------------------------------------------------

SOPClassUIDSplitter::~SOPClassUIDSplitter()
{
}

//-----------------------------------------------------------------------------

std::string SOPClassUIDSplitter::getName() const
{
    return SOPClassUIDSplitter::s_FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string SOPClassUIDSplitter::getDescription() const
{
    return SOPClassUIDSplitter::s_FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool SOPClassUIDSplitter::isConfigurationRequired() const
{
    return false;
}

//-----------------------------------------------------------------------------

SOPClassUIDSplitter::DicomSeriesContainerType SOPClassUIDSplitter::apply(
    const ::fwMedData::DicomSeries::sptr& series,
    const ::fwLog::Logger::sptr& logger) const
{
    const DicomSeriesContainerType result = ::fwDicomIOFilter::splitter::TagValueSplitter::apply(series, logger);

    for(const ::fwMedData::DicomSeries::sptr& dicomSeries :  result)
    {
        DcmFileFormat fileFormat;
        OFCondition status;
        DcmDataset* dataset;
        OFString data;

        // Open first instance
        const auto firstItem                           = dicomSeries->getDicomContainer().begin();
        const ::fwMemory::BufferObject::sptr bufferObj = firstItem->second;
        const size_t buffSize                          = bufferObj->getSize();
        const std::string dicomPath                    = bufferObj->getStreamInfo().fsFile.string();
        ::fwMemory::BufferObject::Lock lock(bufferObj);
        char* buffer = static_cast< char* >( lock.getBuffer() );

        DcmInputBufferStream is;
        is.setBuffer(buffer, offile_off_t(buffSize));
        is.setEos();

        fileFormat.transferInit();
        if (!fileFormat.read(is).good())
        {
            FW_RAISE("Unable to read Dicom file '"<< dicomPath <<"' "<<
                     "(slice: '" << firstItem->first << "')");
        }

        fileFormat.loadAllDataIntoMemory();
        fileFormat.transferEnd();

        dataset = fileFormat.getDataset();

        // Read SOPClassUID
        dataset = fileFormat.getDataset();
        status  = dataset->findAndGetOFStringArray(DCM_SOPClassUID, data);
        FW_RAISE_IF("Unable to read tags: \""+dicomPath+"\"", status.bad());

        ::fwMedData::DicomSeries::SOPClassUIDContainerType sopClassUIDContainer;
        sopClassUIDContainer.insert(data.c_str());
        dicomSeries->setSOPClassUIDs(sopClassUIDContainer);
    }

    if(result.size() > 1)
    {
        logger->warning("The same series instance UID has been used for several instances "
                        "with different SOP class UID. The series has been split.");
    }

    return result;
}

} // namespace splitter
} // namespace fwDicomIOFilter

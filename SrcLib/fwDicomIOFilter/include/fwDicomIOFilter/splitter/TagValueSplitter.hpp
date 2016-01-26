/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_SPLITTER_TAGVALUESPLITTER_HPP__
#define __FWDICOMIOFILTER_SPLITTER_TAGVALUESPLITTER_HPP__

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/splitter/ISplitter.hpp"


#include <dcmtk/dcmdata/dctagkey.h>

#include <fwDicomData/DicomSeries.hpp>

#include <dcmtk/dcmdata/dctagkey.h>

namespace fwDicomIOFilter
{
namespace splitter
{

/**
 * @class TagValueSplitter
 * @brief Filter that uses a tag to split the instances
 * @date  2014.
 */
class FWDICOMIOFILTER_CLASS_API TagValueSplitter : public ISplitter
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (TagValueSplitter)(ISplitter),
                                            (()), ::fwDicomIOFilter::factory::New< TagValueSplitter > );

    /// Constructor
    FWDICOMIOFILTER_API TagValueSplitter(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTER_API virtual ~TagValueSplitter();

    /// Override
    FWDICOMIOFILTER_API virtual DicomSeriesContainerType apply(
        const ::fwDicomData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger) const
    throw(::fwDicomIOFilter::exceptions::FilterFailure);

    /// Return the name of the filter
    FWDICOMIOFILTER_API virtual std::string getName() const;

    /// Return the description of the filter
    FWDICOMIOFILTER_API virtual std::string getDescription() const;

    /// Return true if a configuration is required
    FWDICOMIOFILTER_API virtual bool isConfigurationRequired() const;

    /**
     * @brief Tag used to sort instances
     * @{ */
    const DcmTagKey getTag () const
    {
        return m_tag;
    }
    DcmTagKey& getRefTag ()
    {
        return this->m_tag;
    }
    const DcmTagKey &getCRefTag() const
    {
        return this->m_tag;
    }
    void setTag (const DcmTagKey& _tag)
    {
        this->m_tag = _tag;
    }
    /**  @} */

protected:
    /// Filter name
    static const std::string s_FILTER_NAME;

    /// Filter description
    static const std::string s_FILTER_DESCRIPTION;

    /// Tag used to sort instances
    DcmTagKey m_tag;

};

} // namespace splitter
} // namespace fwDicomIOFilter


#endif /* __FWDICOMIOFILTER_SPLITTER_TAGVALUESPLITTER_HPP__ */

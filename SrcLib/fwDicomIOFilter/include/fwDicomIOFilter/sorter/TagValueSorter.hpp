/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_SORTER_TAGVALUESORTER_HPP__
#define __FWDICOMIOFILTER_SORTER_TAGVALUESORTER_HPP__

#include <dcmtk/dcmdata/dctagkey.h>

#include <fwDicomData/DicomSeries.hpp>

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/sorter/ISorter.hpp"

namespace fwDicomIOFilter
{
namespace sorter
{

/**
 * @class TagValueSorter
 * @brief Filter that uses a tag to sort the instances.
 * @date  2014.
 */
class FWDICOMIOFILTER_CLASS_API TagValueSorter : public ISorter
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (TagValueSorter)(ISorter),
                                            (()), ::fwDicomIOFilter::factory::New< TagValueSorter > );

    /// Constructor
    FWDICOMIOFILTER_API TagValueSorter(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTER_API virtual ~TagValueSorter();

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

} // namespace sorter
} // namespace fwDicomIOFilter


#endif /* __FWDICOMIOFILTER_SORTER_TAGVALUESORTER_HPP__ */

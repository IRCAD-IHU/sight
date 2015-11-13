/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_SPLITTER_IMAGEORIENTATIONSPLITTER_HPP__
#define __FWDICOMIOFILTER_SPLITTER_IMAGEORIENTATIONSPLITTER_HPP__

#include "fwDicomIOFilter/splitter/TagValueSplitter.hpp"
#include "fwDicomIOFilter/config.hpp"

namespace fwDicomIOFilter
{
namespace splitter
{

/**
 * @class ImageOrientationSplitter
 * @brief Filter that uses the ImageOrientationPatient tag to split the instances.
 * @date  2014.
 */
class FWDICOMIOFILTER_CLASS_API ImageOrientationSplitter : public ::fwDicomIOFilter::splitter::TagValueSplitter
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ImageOrientationSplitter)(ISplitter),
                                            (()), ::fwDicomIOFilter::factory::New< ImageOrientationSplitter > );

    /// Constructor
    FWDICOMIOFILTER_API ImageOrientationSplitter(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTER_API virtual ~ImageOrientationSplitter();

    /// Return the name of the filter
    FWDICOMIOFILTER_API virtual std::string getName() const;

    /// Return the description of the filter
    FWDICOMIOFILTER_API virtual std::string getDescription() const;

    /// Return true if a configuration is required
    FWDICOMIOFILTER_API virtual bool isConfigurationRequired() const;

protected:
    /// Filter name
    static const std::string s_FILTER_NAME;

    /// Filter description
    static const std::string s_FILTER_DESCRIPTION;

};

} // namespace splitter
} // namespace fwDicomIOFilter


#endif /* __FWDICOMIOFILTER_SPLITTER_IMAGEORIENTATIONSPLITTER_HPP__ */

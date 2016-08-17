/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOTOOLS_SFRAMERESIZER_HPP__
#define __VIDEOTOOLS_SFRAMERESIZER_HPP__

#include "videoTools/config.hpp"

#include <arData/FrameTL.hpp>
#include <arServices/IOperator.hpp>

namespace videoTools
{

/**
 * @brief   Operator that resizes buffer coming from an input arData::FrameTL and pushes them into an output timeline.
 *          This service uses OpenCV to perform the resize processing.
 */
class VIDEOTOOLS_CLASS_API SFrameResizer : public ::arServices::IOperator
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SFrameResizer)(::arServices::IOperator) );

    /// Constructor.
    VIDEOTOOLS_API SFrameResizer() throw();

    /// Destructor. Does nothing
    VIDEOTOOLS_API virtual ~SFrameResizer() throw();

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="SFrameResizerInstance" impl="::videoTools::SFrameResizer" type="::arServices::IOperator">
             <input>frameTL</input>
             <output>resizedFrameTL</output>
             <ratio x="50" y="50" />
             <interpolation>linear</interpolation>
        </service>
       @endcode
     * - \b input : key of the timeline used as input.
     * - \b output : key of the timeline used as input.
     * - \b ratio : ratio (x,y) of the resize operation as a percentage.
     * - \b interpolation(optional) : method to use amongst : nearest(default), linear, area, cubic, lanczos4.
     */
    VIDEOTOOLS_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Grab timelines.
    VIDEOTOOLS_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Stopping method.
    VIDEOTOOLS_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Called when a new buffer is pushed inside the input timeline. A new buffer containing the
    /// resied image is pushed inside the output timeline.
    VIDEOTOOLS_API virtual void updating() throw ( ::fwTools::Failed );

private:

    /// Frame timeline key
    std::string m_inputTLKey;

    /// Frame timeline key
    std::string m_outputTLKey;

    /// Frame timeline
    ::arData::FrameTL::sptr m_inputTL;

    /// Frame timeline
    ::arData::FrameTL::sptr m_outputTL;

    /// Resize ratio
    float m_ratio[2];

    /// Interpolation method - as defined in OpenCV
    int m_interpolationMethod;

    /// Connections
    ::fwServices::helper::SigSlotConnection m_connections;
};

} // videoTools

#endif // __VIDEOTOOLS_SFRAMERESIZER_HPP__

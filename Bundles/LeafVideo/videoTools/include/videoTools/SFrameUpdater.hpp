/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOTOOLS_SFRAMEUPDATER_HPP__
#define __VIDEOTOOLS_SFRAMEUPDATER_HPP__

#include "videoTools/config.hpp"

#include <extData/FrameTL.hpp>

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Image.hpp>
#include <fwCore/HiResClock.hpp>
#include <fwServices/IController.hpp>

namespace videoTools
{

/**
 * @brief   Service used to update frame and trigger render once timelines have been updated.
 * @class   SFrameUpdater
 *
 */
class VIDEOTOOLS_CLASS_API SFrameUpdater : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SFrameUpdater)(::fwServices::IController) );

    static const ::fwCom::Slots::SlotKeyType s_UPDATE_FRAME_SLOT;
    typedef ::fwCom::Slot<void (::fwCore::HiResClock::HiResClockType)> UpdateFrameSlotType;

    /// Type of signal m_sigRenderRequested
    typedef ::fwCom::Signal< void () > RenderRequestedSignalType;

    /// Key in m_signals map of signal m_sigObjectModified
    static const ::fwCom::Signals::SignalKeyType s_RENDER_REQUESTED_SIG;

    /// Constructor
    VIDEOTOOLS_API SFrameUpdater() throw();

    /// Destructorin
    VIDEOTOOLS_API virtual ~SFrameUpdater() throw();

protected:

    /// Starting method. Initialize timer.
    VIDEOTOOLS_API virtual void starting() throw(fwTools::Failed);

    /**
     * @brief Configuring method.
     *
     * @code{.xml}
       <service impl="::videoTools::SFrameUpdater">
        <config>
            <frameTLKey>frameTL</frameTLKey>
            <imageKey>image</imageKey>
        </config>
       </service>
       @endcode
     * - \b frameTLKey represents the key of the frame timeline contained in this service composite.
     * - \b imageKey represents the key of the image contained in the visu composite.
     */
    VIDEOTOOLS_API virtual void configuring() throw(::fwTools::Failed);

    /// Stopping method.
    VIDEOTOOLS_API virtual void stopping() throw(fwTools::Failed);

    /// Updating method. Deforms the mesh
    VIDEOTOOLS_API virtual void updating() throw(fwTools::Failed);

    /// Update frame slots
    VIDEOTOOLS_API virtual void updateFrame( ::fwCore::HiResClock::HiResClockType timestamp );

    /// Update the image according to the frame timeline.
    VIDEOTOOLS_API virtual void updateImage();

    /// Request Render
    VIDEOTOOLS_API void requestRender();

private:

    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

    /// Slots used when the frame have been refreshed
    UpdateFrameSlotType::sptr m_slotUpdateFrame;

    /// Signal that emits signal when refresh is requested
    RenderRequestedSignalType::sptr m_sigRenderRequested;

    /// Connections
    ::fwServices::helper::SigSlotConnection::sptr m_connections;

    /// Frame timeline key
    std::string m_frameTLKey;

    /// Frame timeline
    ::extData::FrameTL::csptr m_frameTL;

    /// Image key
    std::string m_imageKey;

    /// Image
    ::fwData::Image::sptr m_image;

    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// Last matrix updated key
    std::string m_lastMatrixUpdatedKey;

    /// Hight resolution timer to log information about computing function time
    ::fwCore::HiResTimer m_hiRestimer;

    bool m_imageInitialized;
};

} //namespace videoTools

#endif /* __VIDEOTOOLS_SFRAMEUPDATER_HPP__ */

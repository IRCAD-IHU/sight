/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/player/VideoRegistry.hpp"

#include "videoQt/SFrameGrabber.hpp"

#include <fwCore/base.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>
#include <fwComEd/helper/Image.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwTools/Type.hpp>
#include <fwServices/macros.hpp>

#include <extData/FrameTL.hpp>
#include <arData/Camera.hpp>

#include <QImage>
#include <QSize>
#include <QVideoFrame>

#include <boost/filesystem/operations.hpp>

namespace videoQt
{

//-----------------------------------------------------------------------------


fwServicesRegisterMacro( ::fwServices::IController, ::videoQt::SFrameGrabber, ::extData::FrameTL);

const ::fwCom::Signals::SignalKeyType SFrameGrabber::s_POSITION_MODIFIED_SIG = "positionModified";
const ::fwCom::Signals::SignalKeyType SFrameGrabber::s_DURATION_MODIFIED_SIG = "durationModified";

const ::fwCom::Slots::SlotKeyType SFrameGrabber::s_START_CAMERA_SLOT       = "startCamera";
const ::fwCom::Slots::SlotKeyType SFrameGrabber::s_STOP_CAMERA_SLOT        = "stopCamera";
const ::fwCom::Slots::SlotKeyType SFrameGrabber::s_PAUSE_CAMERA_SLOT       = "pauseCamera";
const ::fwCom::Slots::SlotKeyType SFrameGrabber::s_LOOP_VIDEO_SLOT         = "loopVideo";
const ::fwCom::Slots::SlotKeyType SFrameGrabber::s_SET_POSITION_VIDEO_SLOT = "setPositionVideo";

//-----------------------------------------------------------------------------

SFrameGrabber::SFrameGrabber() throw() : m_loopVideo(false),
                                         m_videoPlayer(nullptr),
                                         m_horizontallyFlip(false),
                                         m_verticallyFlip(false)
{
    newSignal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
    newSignal< DurationModifiedSignalType >( s_DURATION_MODIFIED_SIG );

    newSlot( s_START_CAMERA_SLOT, &SFrameGrabber::startCamera, this );
    newSlot( s_STOP_CAMERA_SLOT, &SFrameGrabber::stopCamera, this );
    newSlot( s_PAUSE_CAMERA_SLOT, &SFrameGrabber::pauseCamera, this );
    newSlot( s_LOOP_VIDEO_SLOT, &SFrameGrabber::toggleLoopMode, this );
    newSlot( s_SET_POSITION_VIDEO_SLOT, &SFrameGrabber::setPosition, this );
}

//-----------------------------------------------------------------------------

SFrameGrabber::~SFrameGrabber() throw()
{
}

//-----------------------------------------------------------------------------

void SFrameGrabber::starting() throw(::fwTools::Failed)
{
    SLM_ASSERT("m_videoPlayer must be null - have you called starting() twice ?", nullptr == m_videoPlayer);
}

//-----------------------------------------------------------------------------

void SFrameGrabber::stopping() throw(::fwTools::Failed)
{
    this->stopCamera();
}

//-----------------------------------------------------------------------------

void SFrameGrabber::configuring()  throw ( ::fwTools::Failed )
{
    if(!this->isVersion2())
    {
        ::fwRuntime::ConfigurationElement::sptr cameraCfg = m_configuration->findConfigurationElement("cameraFwId");
        SLM_ASSERT("Missing tag 'cameraFwId'", cameraCfg);
        m_cameraID = cameraCfg->getValue();
        SLM_ASSERT("tag 'cameraFwId' must not be empty", !m_cameraID.empty());
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::updating() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SFrameGrabber::startCamera()
{
    ::arData::Camera::csptr camera           = this->getCamera();
    ::arData::Camera::SourceType eSourceType = camera->getCameraSource();

    // Make sure the user has selected a valid source
    if( ::arData::Camera::UNKNOWN != eSourceType )
    {
        this->stopCamera();
        this->setMirror(false, false);

    #ifdef WIN32
        if( ::arData::Camera::DEVICE == eSourceType )
        {
            this->setMirror(false, true);
        }
    #endif

        player::VideoRegistry& registry = player::VideoRegistry::getInstance();
        m_videoPlayer = registry.requestPlayer(camera);
        if(m_videoPlayer)
        {
            QObject::connect(m_videoPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));
            QObject::connect(m_videoPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));
            QObject::connect(m_videoPlayer, SIGNAL(frameAvailable(QVideoFrame)), this, SLOT(presentFrame(QVideoFrame)));

            m_videoPlayer->play();
        }
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::pauseCamera()
{
    // because of the requestPlayer/releasePlayer mechanism, the m_videoPlayer may be invalid when the user presses the "pause" button
    if(m_videoPlayer)
    {
        m_videoPlayer->pause();
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::stopCamera()
{
    // because of the requestPlayer/releasePlayer mechanism, the m_videoPlayer may be invalid when the user presses the "pause" button
    if(m_videoPlayer)
    {
        m_videoPlayer->stop();

        auto sigPosition = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
        sigPosition->asyncEmit(static_cast<std::int64_t>(-1));

        auto sigDuration = this->signal< DurationModifiedSignalType >( s_DURATION_MODIFIED_SIG );
        sigDuration->asyncEmit(static_cast<std::int64_t>(-1));

        QObject::disconnect(m_videoPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));
        QObject::disconnect(m_videoPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));
        QObject::disconnect(m_videoPlayer, SIGNAL(frameAvailable(QVideoFrame)), this, SLOT(presentFrame(QVideoFrame)));

        player::VideoRegistry& registry = player::VideoRegistry::getInstance();
        registry.releasePlayer(m_videoPlayer);

        m_videoPlayer = nullptr;

        // Reset the timeline and send a black frame
        ::extData::FrameTL::sptr timeline;
        if(this->isVersion2())
        {
            timeline = this->getInOut< ::extData::FrameTL >("frameTL");
        }
        else
        {
            timeline = this->getObject< ::extData::FrameTL >();
        }
        const ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec() + 1;
        SPTR(::extData::FrameTL::BufferType) buffer = timeline->createBuffer(timestamp);
        ::boost::uint8_t* destBuffer                = reinterpret_cast< ::boost::uint8_t* >( buffer->addElement(0) );

        std::fill(destBuffer,
                  destBuffer + timeline->getWidth() * timeline->getHeight() * timeline->getNumberOfComponents(), 0);

        // push buffer and notify
        timeline->clearTimeline();
        timeline->pushObject(buffer);

        auto sigTL = timeline->signal< ::extData::TimeLine::ObjectPushedSignalType >(
            ::extData::TimeLine::s_OBJECT_PUSHED_SIG );
        sigTL->asyncEmit(timestamp);
    }
}

//-----------------------------------------------------------------------------

CSPTR(::arData::Camera) SFrameGrabber::getCamera()
{
    ::arData::Camera::csptr camera;
    if(this->isVersion2())
    {
        camera = this->getInput< ::arData::Camera>("camera");
    }
    else
    {
        ::fwTools::Object::csptr obj = ::fwTools::fwID::getObject(m_cameraID);
        camera                       = ::arData::Camera::dynamicConstCast(obj);
        FW_RAISE_IF("Camera not found", !camera);
    }
    return camera;
}

//-----------------------------------------------------------------------------

void SFrameGrabber::toggleLoopMode()
{
    // because of the requestPlayer/releasePlayer mechanism, the m_videoPlayer may be invalid when the user presses the "pause" button
    if(m_videoPlayer)
    {
        m_loopVideo = !m_loopVideo;
        m_videoPlayer->toggleLoopMode(m_loopVideo);
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::setPosition(std::int64_t position)
{
    // because of the requestPlayer/releasePlayer mechanism, the m_videoPlayer may be invalid when the user presses the "pause" button
    if(m_videoPlayer)
    {
        m_videoPlayer->setPosition(position);
    }
}

//-----------------------------------------------------------------------------

void SFrameGrabber::onPositionChanged(qint64 position)
{
    auto sig = this->signal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
    sig->asyncEmit(static_cast<std::int64_t>(position));
}

//----------------------------------------------------------------------------

void SFrameGrabber::onDurationChanged(qint64 duration)
{
    auto sig = this->signal< DurationModifiedSignalType >( s_DURATION_MODIFIED_SIG );
    sig->asyncEmit(static_cast<std::int64_t>(duration));
}

//----------------------------------------------------------------------------

void SFrameGrabber::presentFrame(const QVideoFrame& frame)
{
    if( frame.pixelFormat() == QVideoFrame::Format_Invalid )
    {
        SLM_WARN("Dropped frame");
        return;
    }

    ::extData::FrameTL::sptr timeline;
    if(this->isVersion2())
    {
        timeline = this->getInOut< ::extData::FrameTL >("frameTL");
    }
    else
    {
        timeline = this->getObject< ::extData::FrameTL >();
    }

    // If we have the same output format, we can take the fast path
    const int width  = frame.width();
    const int height = frame.height();

    if(static_cast<unsigned int>(height) != timeline->getHeight() ||
       static_cast<unsigned int>(width) != timeline->getWidth())
    {
        timeline->initPoolSize(static_cast<size_t>(width), static_cast<size_t>(height), ::fwTools::Type::s_UINT8, 4);
    }

    // This is called on a different worker than the service, so we must lock the frame
    ::fwCore::mt::ReadLock lock(m_videoFrameMutex);

    const ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();

    SPTR(::extData::FrameTL::BufferType) buffer = timeline->createBuffer(timestamp);
    std::uint64_t* destBuffer = reinterpret_cast< std::uint64_t* >( buffer->addElement(0) );

    SLM_ASSERT("Pixel format must be RGB32", frame.pixelFormat() == QVideoFrame::Format_RGB32 ||
               frame.pixelFormat() == QVideoFrame::Format_ARGB32_Premultiplied ||
               frame.pixelFormat() == QVideoFrame::Format_ARGB32);

    // const_cast required to call function map() - but it's safe, since we map for READ_ONLY access !
    QVideoFrame& mappedFrame = const_cast< QVideoFrame& >( frame );
    // Make sure the frame has been mapped before accessing .bits() !!
    mappedFrame.map(QAbstractVideoBuffer::ReadOnly);

    const std::uint64_t* frameBuffer;

    // Keep this QImage in the global scope, until we are done with it !
    QImage imgFlipped;

    if( m_horizontallyFlip || m_verticallyFlip )
    {
        imgFlipped = QImage(mappedFrame.bits(),
                            width,
                            height,
                            QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat()))
                     .mirrored(m_horizontallyFlip, m_verticallyFlip);

        frameBuffer = reinterpret_cast< const std::uint64_t *>( imgFlipped.bits() );
    }
    else
    {
        frameBuffer = reinterpret_cast< const std::uint64_t *>( mappedFrame.bits() );
    }

    // Unmap when we don't need access to .bits() any longer
    mappedFrame.unmap();

    const unsigned int size = static_cast<unsigned int>(width*height) >> 1;

    // Buffer conversion: Qt returns frame in ARGB format, but we need RGBA.
    for(unsigned int idx = 0; idx < size; ++idx)
    {
        const std::uint64_t pixel = *frameBuffer++;

        *destBuffer++ = 0xFF000000FF000000 | (pixel & 0x0000FF000000FF00) |
                        (pixel & 0x000000FF000000FF) << 16 | (pixel & 0x00FF000000FF0000) >> 16;
    }

    // push buffer and notify
    timeline->pushObject(buffer);

    ::extData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = timeline->signal< ::extData::TimeLine::ObjectPushedSignalType >(::extData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

} // namespace videoQt


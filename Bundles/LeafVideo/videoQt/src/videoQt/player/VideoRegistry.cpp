/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoQt/player/VideoRegistry.hpp"
#include "videoQt/player/QVideoPlayer.hpp"

#include "videoQt/helper/preferences.hpp"

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwCore/spyLog.hpp>
#include <fwCore/exceptionmacros.hpp>

namespace videoQt
{
namespace player
{

//-----------------------------------------------------------------------------

VideoRegistry& VideoRegistry::getInstance()
{
    static VideoRegistry instance;
    return instance;
}

//-----------------------------------------------------------------------------

VideoRegistry::VideoRegistry() : m_mapVideoPlayer(Key::less), m_mapRefCount(Key::less)
{
}

//-----------------------------------------------------------------------------

VideoRegistry::~VideoRegistry()
{
    SLM_ASSERT("VideoPlayer map must be empty.", m_mapVideoPlayer.size() == 0 );
}

//-----------------------------------------------------------------------------

QVideoPlayer* VideoRegistry::requestPlayer(const ::arData::Camera::sptr& camera)
{
    const ::arData::Camera::SourceType type = camera->getCameraSource();

    Key videoKey;
    videoKey.m_eType = type;
    switch(type)
    {
        case ::arData::Camera::FILE:
            videoKey.m_strID = camera->getVideoFile().string();
            break;
        case ::arData::Camera::STREAM:
            videoKey.m_strID = camera->getStreamUrl();
            break;
        case ::arData::Camera::DEVICE:
            videoKey.m_strID = camera->getCameraID();
            break;
        default:
            SLM_ASSERT("Unknown camera source type", false);
    }

    MapVideoPlayerType::const_iterator playerIter = m_mapVideoPlayer.find(videoKey);
    MapRefCountType::const_iterator refCountIter  = m_mapRefCount.find(videoKey);
    std::uint8_t nbRef                            = 1;

    QVideoPlayer* player;
    if(playerIter != m_mapVideoPlayer.end())
    {
        nbRef  = refCountIter->second + 1;
        player = playerIter->second;
    }
    else
    {
        player = new QVideoPlayer();
        switch(type)
        {
            case ::arData::Camera::FILE:
            {
                /// Path of the video file stored in the camera description
                ::boost::filesystem::path videoPath(camera->getVideoFile());
                ::boost::filesystem::path videoDir(::videoQt::helper::getVideoDir());

                // For compatibility with old calibration with absolute path
                if (!videoPath.is_absolute())
                {
                    videoPath = videoDir / videoPath;
                }

                try
                {
                    player->initCameraFile(videoPath);
                }
                catch(std::exception& e)
                {
                    delete player;
                    ::fwGui::dialog::MessageDialog::showMessageDialog(
                        "Camera error",
                        e.what(),
                        ::fwGui::dialog::IMessageDialog::WARNING);
                    return nullptr;
                }

                break;
            }
            case ::arData::Camera::STREAM:
                player->initCameraStream(camera->getStreamUrl());
                break;
            case ::arData::Camera::DEVICE:
                player->initCameraDevice(camera->getCameraID(),
                                         camera->getWidth(), camera->getHeight(),
                                         camera->getMaximumFrameRate());
                break;
            default:
                SLM_FATAL("Unknown camera source type");
        }
        m_mapVideoPlayer.insert(MapVideoPlayerType::value_type(videoKey, player));
    }
    m_mapRefCount[videoKey] = nbRef;

    return player;
}

//-----------------------------------------------------------------------------

void VideoRegistry::releasePlayer(QVideoPlayer* player)
{
    for(const MapVideoPlayerType::value_type elt : m_mapVideoPlayer)
    {
        if(elt.second == player)
        {
            MapRefCountType::const_iterator refCountIter = m_mapRefCount.find(elt.first);
            std::uint8_t nbRef                           = refCountIter->second;
            nbRef--;
            if(nbRef  > 0)
            {
                m_mapRefCount[elt.first] = nbRef;
            }
            else
            {
                m_mapVideoPlayer.erase(elt.first);
                m_mapRefCount.erase(elt.first);
                delete player;
            }
            return;
        }
    }
    SLM_FATAL("Couldn't not release VideoPlayer.");
}

//-----------------------------------------------------------------------------

} //namespace player
} //namespace videoQt

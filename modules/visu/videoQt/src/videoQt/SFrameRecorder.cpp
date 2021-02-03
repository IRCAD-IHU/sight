/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "videoQt/SFrameRecorder.hpp"

#include <core/com/Slots.hxx>

#include <data/Composite.hpp>
#include <data/Image.hpp>
#include <data/Point.hpp>
#include <data/tools/helper/Vector.hpp>

#include <services/IController.hpp>
#include <services/macros.hpp>

#include <QImage>
#include <QString>

#include <filesystem>

namespace videoQt
{

fwServicesRegisterMacro( ::sight::services::IController, ::videoQt::SFrameRecorder, ::sight::data::FrameTL)

//-----------------------------------------------------------------------------

const core::com::Slots::SlotKeyType SFrameRecorder::s_SAVE_FRAME_SLOT = "saveFrame";
const core::com::Slots::SlotKeyType SFrameRecorder::s_START_RECORD_SLOT = "startRecord";
const core::com::Slots::SlotKeyType SFrameRecorder::s_STOP_RECORD_SLOT  = "stopRecord";
const core::com::Slots::SlotKeyType SFrameRecorder::s_PAUSE_RECORD_SLOT = "pauseRecord";

static const services::IService::KeyType s_FRAMETL_INPUT = "frameTL";

//-----------------------------------------------------------------------------

SFrameRecorder::SFrameRecorder() noexcept :
    m_count(0),
    m_isRecording(false),
    m_isPaused(false)
{
    m_slotSaveFrame   = core::com::newSlot( &SFrameRecorder::saveFrame, this);
    m_slotStartRecord = core::com::newSlot( &SFrameRecorder::startRecord, this);
    m_slotStopRecord  = core::com::newSlot( &SFrameRecorder::stopRecord, this);
    m_slotPauseRecord = core::com::newSlot( &SFrameRecorder::pauseRecord, this);

    core::com::HasSlots::m_slots(s_SAVE_FRAME_SLOT, m_slotSaveFrame)
        (s_START_RECORD_SLOT, m_slotStartRecord)
        (s_STOP_RECORD_SLOT, m_slotStopRecord)
        (s_PAUSE_RECORD_SLOT, m_slotPauseRecord);

    core::com::HasSlots::m_slots.setWorker(m_associatedWorker);
}

//-----------------------------------------------------------------------------

SFrameRecorder::~SFrameRecorder() noexcept
{
}

//-----------------------------------------------------------------------------

void SFrameRecorder::starting()
{
    std::filesystem::create_directories(m_path);
}

//-----------------------------------------------------------------------------

void SFrameRecorder::stopping()
{
}

//-----------------------------------------------------------------------------

void SFrameRecorder::configuring()
{
    ConfigurationType config = m_configuration->findConfigurationElement("path");
    SLM_ASSERT("Missing <path> tag", config);

    m_path = config->getValue();
}

//-----------------------------------------------------------------------------

void SFrameRecorder::updating()
{
}

//-----------------------------------------------------------------------------

void SFrameRecorder::saveFrame(core::HiResClock::HiResClockType timestamp)
{
    if (m_isRecording && !m_isPaused)
    {
        data::FrameTL::csptr frameTL = this->getInput< data::FrameTL >(s_FRAMETL_INPUT);

        CSPTR(data::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(timestamp);
        SLM_WARN_IF("No frame found in timeline for timestamp : " << timestamp, !buffer);

        if(buffer)
        {
            int width  = static_cast<int>(frameTL->getWidth());
            int height = static_cast<int>(frameTL->getHeight());
            QImage image(width, height, QImage::Format_ARGB32);

            std::uint64_t* imageBuffer       = reinterpret_cast< std::uint64_t*>( image.bits() );
            const std::uint64_t* frameBuffer =
                reinterpret_cast< const std::uint64_t*>( &buffer->getElement(0) );

            const unsigned int size = static_cast<unsigned int>(width * height) >> 1;

            for(unsigned int idx = 0; idx < size; ++idx)
            {
                const std::uint64_t pixel = *frameBuffer++;

                *imageBuffer++ = (pixel & 0xFF00FF00FF00FF00) | (pixel & 0x000000FF000000FF) << 16
                                 | (pixel & 0x00FF000000FF0000) >> 16;
            }

            std::stringstream str;
            str.width(8);
            str.fill('0');
            str << ++m_count;
            std::string filename(m_path.string() + "/" + "img_" + str.str() + ".bmp");
            image.save(QString::fromStdString(filename));
        }
    }
}

//------------------------------------------------------------------------------

void SFrameRecorder::startRecord()
{
    data::FrameTL::csptr frameTL = this->getInput< data::FrameTL >(s_FRAMETL_INPUT);

    if (frameTL->getType() != core::tools::Type::s_UINT8 || frameTL->getNumberOfComponents() != 4)
    {
        SLM_ERROR("Frame type not managed. Only image with type 'uint8' with 4 components are managed.");
        return;
    }

    m_connections.disconnect();
    m_connections.connect(frameTL, data::FrameTL::s_OBJECT_PUSHED_SIG,
                          this->getSptr(), s_SAVE_FRAME_SLOT);
    m_isRecording = true;
    m_isPaused    = false;
}

//------------------------------------------------------------------------------

void SFrameRecorder::stopRecord()
{
    m_connections.disconnect();
    m_isRecording = false;
}
//------------------------------------------------------------------------------

void SFrameRecorder::pauseRecord()
{
    if (m_isRecording && !m_isPaused)
    {
        m_connections.disconnect();
        m_isPaused = true;
    }
    else if (m_isRecording && m_isPaused)
    {
        m_connections.connect(this->getInput< data::FrameTL >(s_FRAMETL_INPUT),
                              data::FrameTL::s_OBJECT_PUSHED_SIG,
                              this->getSptr(), s_SAVE_FRAME_SLOT);

        m_isPaused = false;
    }
}

//------------------------------------------------------------------------------

} // namespace videoQt

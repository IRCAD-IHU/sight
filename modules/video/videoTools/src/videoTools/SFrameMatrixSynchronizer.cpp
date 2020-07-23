/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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

#include "videoTools/SFrameMatrixSynchronizer.hpp"

#include <arData/FrameTL.hpp>
#include <arData/MatrixTL.hpp>
#include <arData/timeline/Buffer.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwMath/Compare.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

#include <algorithm>
#include <functional>

fwServicesRegisterMacro(::arServices::ISynchronizer, ::videoTools::SFrameMatrixSynchronizer)

namespace videoTools
{

const ::fwCom::Signals::SignalKeyType SFrameMatrixSynchronizer::s_SYNCHRONIZATION_DONE_SIG    = "synchronizationDone";
const ::fwCom::Signals::SignalKeyType SFrameMatrixSynchronizer::s_SYNCHRONIZATION_SKIPPED_SIG =
    "synchronizationSkipped";
const ::fwCom::Signals::SignalKeyType SFrameMatrixSynchronizer::s_ALL_MATRICES_FOUND_SIG    = "allMatricesFound";
const ::fwCom::Signals::SignalKeyType SFrameMatrixSynchronizer::s_MATRIX_SYNCHRONIZED_SIG   = "matrixSynchronized";
const ::fwCom::Signals::SignalKeyType SFrameMatrixSynchronizer::s_MATRIX_UNSYNCHRONIZED_SIG = "matrixUnsynchronized";

const ::fwServices::IService::KeyType s_FRAMETL_INPUT  = "frameTL";
const ::fwServices::IService::KeyType s_MATRIXTL_INPUT = "matrixTL";
const ::fwServices::IService::KeyType s_IMAGE_INOUT    = "image";
const ::fwServices::IService::KeyType s_MATRICES_INOUT = "matrices";

// Private slot
const ::fwCom::Slots::SlotKeyType s_RESET_TIMELINE_SLOT = "reset";

// Public slots
const ::fwCom::Slots::SlotKeyType SFrameMatrixSynchronizer::s_SYNCHRONIZE_SLOT     = "synchronize";
const ::fwCom::Slots::SlotKeyType SFrameMatrixSynchronizer::s_SET_FRAME_DELAY_SLOT = "setFrameDelay";

// ----------------------------------------------------------------------------

SFrameMatrixSynchronizer::SFrameMatrixSynchronizer() noexcept :
    m_tolerance(500.),
    m_imagesInitialized(false),
    m_timeStep(33),
    m_lastTimestamp(std::numeric_limits<double>::lowest())
{
    m_sigSynchronizationDone    = newSignal<SynchronizationDoneSignalType>(s_SYNCHRONIZATION_DONE_SIG);
    m_sigSynchronizationSkipped = newSignal<synchronizationSkippedSignalType>(s_SYNCHRONIZATION_SKIPPED_SIG);
    m_sigAllMatricesFound       = newSignal<AllMatricesFoundSignalType>(s_ALL_MATRICES_FOUND_SIG);
    m_sigMatrixSynchronized     = newSignal<MatrixSynchronizedSignalType>(s_MATRIX_SYNCHRONIZED_SIG);
    m_sigMatrixUnsynchronized   = newSignal<MatrixUnsynchronizedSignalType>(s_MATRIX_UNSYNCHRONIZED_SIG);

    newSlot(s_RESET_TIMELINE_SLOT, &SFrameMatrixSynchronizer::resetTimeline, this);
    newSlot(s_SYNCHRONIZE_SLOT, &SFrameMatrixSynchronizer::synchronize, this);
    newSlot(s_SET_FRAME_DELAY_SLOT, &SFrameMatrixSynchronizer::setFrameDelay, this);
}

//-----------------------------------------------------------------------------

SFrameMatrixSynchronizer::~SFrameMatrixSynchronizer() noexcept
{
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SFrameMatrixSynchronizer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_FRAMETL_INPUT, ::arData::TimeLine::s_CLEARED_SIG, s_RESET_TIMELINE_SLOT );
    connections.push( s_FRAMETL_INPUT, ::arData::TimeLine::s_OBJECT_PUSHED_SIG, s_UPDATE_SLOT );
    connections.push( s_MATRIXTL_INPUT, ::arData::TimeLine::s_OBJECT_PUSHED_SIG, s_UPDATE_SLOT );
    return connections;
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::configuring()
{
    const auto cfg = this->getConfigTree();

    const auto framerate = cfg.get<unsigned int>("framerate", 30);
    m_timeStep  = framerate != 0 ? 1000 / cfg.get<unsigned int>("framerate", 30) : 0;
    m_tolerance = cfg.get<unsigned int>("tolerance", 500);

    m_updateMask = framerate != 0 ? OBJECT_RECEIVED : m_updateMask;
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::starting()
{
    const size_t numFrameTLs = this->getKeyGroupSize(s_FRAMETL_INPUT);
    const size_t numImages   = this->getKeyGroupSize(s_IMAGE_INOUT);
    SLM_ASSERT("You should have the same number of 'frameTL' and 'image' keys", numFrameTLs == numImages);

    m_frameTLs.reserve(numFrameTLs);
    m_images.reserve(numFrameTLs);
    for(size_t i = 0; i < numFrameTLs; ++i)
    {
        m_frameTLs.push_back(this->getWeakInput< ::arData::FrameTL>(s_FRAMETL_INPUT, i));
        m_images.push_back(this->getWeakInOut< ::fwData::Image>(s_IMAGE_INOUT, i));
    }

    const size_t numMatrixTLs = this->getKeyGroupSize(s_MATRIXTL_INPUT);

    m_totalOutputMatrices = 0;
    m_matrixTLs.reserve(numMatrixTLs);
    for(size_t i = 0; i < numMatrixTLs; ++i)
    {
        // Get the group corresponding to the 'i' Matrix TimeLine. The name of this group is matrices0 for example.
        // if ever the group is not found 'getKeyGroupSize' will assert.
        const size_t numMatrices = this->getKeyGroupSize(s_MATRICES_INOUT + std::to_string(i));

        m_matrixTLs.push_back(this->getWeakInput< ::arData::MatrixTL>(s_MATRIXTL_INPUT, i));

        std::vector< ::fwData::mt::weak_ptr< ::fwData::TransformationMatrix3D> > matricesVector;
        for(size_t j = 0; j < numMatrices; ++j)
        {
            matricesVector.push_back(
                this->getWeakInOut< ::fwData::TransformationMatrix3D >( s_MATRICES_INOUT + std::to_string(i), j));
        }
        m_totalOutputMatrices += numMatrices;
        m_matrices.push_back(matricesVector);
    }

    // We need to browser the XML tree to check if a matrix (inside a matrixTL) has or not a `sendStatus` configuration
    const ConfigType configuration = this->getConfigTree();
    const auto inoutsConfig        = configuration.equal_range("inout");
    int matrixIndex                = 0;
    for(auto itConfig = inoutsConfig.first; itConfig != inoutsConfig.second; ++itConfig)
    {
        const std::string group = itConfig->second.get<std::string>("<xmlattr>.group", "");
        if(group.find(s_MATRICES_INOUT) != std::string::npos)
        {
            std::vector<int> sendStatus;

            const auto keyConfig = itConfig->second.equal_range("key");
            for(auto itKeyConfig = keyConfig.first; itKeyConfig != keyConfig.second; ++itKeyConfig)
            {
                const bool needToSendStatus = itKeyConfig->second.get<bool>("<xmlattr>.sendStatus", false);
                if(needToSendStatus)
                {
                    // Push the index that will be send through the `matrixSynchronized` signal
                    sendStatus.push_back(matrixIndex);
                    matrixIndex++;
                }
                else
                {
                    // Push back -1 means that we don't want to send its signal later
                    sendStatus.push_back(-1);
                }
            }
            m_sendMatricesStatus.push_back(sendStatus);
        }
    }

    SLM_ASSERT("No valid worker for timer.", m_associatedWorker);
    if(m_timeStep)
    {
        m_timer = m_associatedWorker->createTimer();
        const auto duration = std::chrono::milliseconds(m_timeStep);
        m_timer->setFunction(std::bind( &SFrameMatrixSynchronizer::synchronize, this));
        m_timer->setDuration(duration);
        m_timer->start();
    }
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::stopping()
{
    if(m_timeStep)
    {
        m_timer->stop();
    }

    m_frameTLs.clear();
    m_images.clear();
    m_matrixTLs.clear();
    m_matrices.clear();
    m_sendMatricesStatus.clear();
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::synchronize()
{
    m_updateMask |= SYNC_REQUESTED;

    if( !(m_updateMask & OBJECT_RECEIVED) )
    {
        return;
    }

    // Timestamp reference for the synchronization
    ::fwCore::HiResClock::HiResClockType frameTimestamp = 0;

    // Get timestamp for synchronization
    std::vector<size_t> availableFramesTL;

    // If multiple TLs are set, we want to synchronize their frames together.
    // If TLs are updated, we get the one with the oldest timestamp to synchronize them.
    // In particular case, we could have only one TL updated, we still need to get frames from it.
    // Then we get the one with the newest timestamp and the other ones are not considered.
    for(size_t i = 0; i != m_frameTLs.size(); ++i)
    {
        const auto tl = m_frameTLs[i].lock();
        SLM_ASSERT("Frame TL does not exist", tl);
        ::fwCore::HiResClock::HiResClockType tlTimestamp = tl->getNewerTimestamp();
        if(tlTimestamp > 0)
        {
            // Check if the current TL timestamp and the previous one are closed enough (according to the tolerance)
            if (std::abs(frameTimestamp - tlTimestamp) < m_tolerance)
            {
                // Sets the reference timestamp as the minimum value
                frameTimestamp = std::min(frameTimestamp, tlTimestamp);
                availableFramesTL.push_back(i);
            }
            // Otherwise keep the most recent timestamp as frameTimestamp
            else
            {
                // If the difference between the TLs timestamp is superior to the tolerance
                // we set the reference timestamp as the maximum of them
                frameTimestamp = std::max(frameTimestamp, tlTimestamp);
                availableFramesTL.push_back(i);

                // Now remove all frames that are too far away
                availableFramesTL.erase(std::remove_if(availableFramesTL.begin(), availableFramesTL.end(),
                                                       [ = ](size_t const& idx)
                    {
                        const auto frametl = m_frameTLs[idx].lock();
                        SLM_ASSERT("Frame TL does not exist", frametl);
                        const auto ts = frametl->getNewerTimestamp();
                        return std::abs(frameTimestamp - ts) >= m_tolerance;
                    }), availableFramesTL.end());
            }
        }
        else
        {
            OSLM_INFO("no available frame for timeline 'frame" << i << ".");
        }
    }

    // Now we compute the time stamp available in the matrix timelines starting from the frame timestamp
    ::fwCore::HiResClock::HiResClockType matrixTimestamp = frameTimestamp;

    std::vector<size_t> availableMatricesTL;
    availableMatricesTL.reserve(m_matrixTLs.size());
    for(size_t i = 0; i != m_matrixTLs.size(); ++i)
    {
        const auto tl = m_matrixTLs[i].lock();
        SLM_ASSERT("Matrix TL does not exist", tl);

        ::fwCore::HiResClock::HiResClockType tlTimestamp = tl->getNewerTimestamp();
        if( (tlTimestamp > 0) && (std::abs(frameTimestamp - tlTimestamp) < m_tolerance) )
        {
            matrixTimestamp = std::min(matrixTimestamp, tlTimestamp);
            availableMatricesTL.push_back(i);
        }
        else
        {
            OSLM_INFO_IF("no available matrix for timeline 'matrix" << i << "'.", tlTimestamp > 0);

            // Notify each matrices in the ith TL that they are unsychronized
            for(const int sendStatus : m_sendMatricesStatus[i])
            {
                if(sendStatus != -1)
                {
                    m_sigMatrixUnsynchronized->asyncEmit(sendStatus);
                }
            }
        }
    }

    // Skip synchzonization if nothing has changed or if the synchronizer decided to go back into the past
    if(matrixTimestamp <= m_lastTimestamp)
    {
        // Notify that the synchronization is skipped
        m_sigSynchronizationSkipped->asyncEmit();
        return;
    }

    m_lastTimestamp = matrixTimestamp;

    for(size_t i = 0; i != m_frameTLs.size(); ++i)
    {
        const auto image = m_images[i].lock();
        CSPTR(::arData::FrameTL::BufferType) buffer;
        {
            const auto frameTL = m_frameTLs[i].lock();
            OSLM_ASSERT("Image with index '" << i << "' does not exist", image);

            const ::fwData::Image::Size size = { frameTL->getWidth(), frameTL->getHeight(), 0};
            // Check if image dimensions have changed
            if(size != image->getSize2() || frameTL->getNumberOfComponents() != image->getNumberOfComponents())
            {
                m_imagesInitialized = false;
            }

            if(!m_imagesInitialized)
            {
                ::fwData::Image::PixelFormat format;
                switch (frameTL->getPixelFormat())
                {
                    case ::arData::FrameTL::PixelFormat::GRAY_SCALE:
                        format = ::fwData::Image::GRAY_SCALE;
                        break;
                    case ::arData::FrameTL::PixelFormat::RGB:
                        format = ::fwData::Image::RGB;
                        break;
                    case ::arData::FrameTL::PixelFormat::BGR:
                        format = ::fwData::Image::BGR;
                        break;
                    case ::arData::FrameTL::PixelFormat::RGBA:
                        format = ::fwData::Image::RGBA;
                        break;
                    case ::arData::FrameTL::PixelFormat::BGRA:
                        format = ::fwData::Image::BGRA;
                        break;
                    default:
                        format = ::fwData::Image::UNDEFINED;
                        FW_DEPRECATED_MSG("FrameTL pixel format should be defined, we temporary assume that the format "
                                          "is GrayScale, RGB or RGBA according to the number of components.", "22.0");
                        // FIXME Support old FrameTL API (sight 22.0)
                        switch (frameTL->getNumberOfComponents())
                        {
                            case 1:
                                format = ::fwData::Image::GRAY_SCALE;
                                break;
                            case 3:
                                format = ::fwData::Image::RGB;
                                break;
                            case 4:
                                format = ::fwData::Image::RGBA;
                                break;
                            default:
                                SLM_ERROR("Number of component not managed.")
                                return;
                        }
                }
                image->resize(size, frameTL->getType(), format);
                const ::fwData::Image::Origin origin = {0., 0., 0.};
                image->setOrigin2(origin);
                const ::fwData::Image::Spacing spacing = {1., 1., 1.};
                image->setSpacing2(spacing);
                image->setWindowWidth(1);
                image->setWindowCenter(0);

                m_imagesInitialized = true;
            }

            buffer = frameTL->getClosestBuffer(matrixTimestamp - m_delay);
        }

        if(!buffer)
        {
            OSLM_INFO("Buffer not found for timestamp "<< matrixTimestamp << " in timeline 'frame" << i << "'.");
            continue;
        }
        const std::uint8_t* frameBuff = &buffer->getElement(0);
        auto iter                     = image->begin<std::uint8_t>();
        std::copy( frameBuff, frameBuff+buffer->getSize(), iter);

        // Notify
        auto sig = image->signal< ::fwData::Image::BufferModifiedSignalType >(
            ::fwData::Image::s_BUFFER_MODIFIED_SIG );
        sig->asyncEmit();
    }

    bool matrixFound       = false;
    size_t syncMatricesNbr = 0;
    for(const auto& tlIdx: availableMatricesTL)
    {
        CSPTR(::arData::MatrixTL::BufferType) buffer;
        {
            const auto matrixTL = m_matrixTLs[tlIdx].lock();
            buffer = matrixTL->getClosestBuffer(matrixTimestamp);
        }

        if(buffer)
        {
            const auto& matrixVector = m_matrices[tlIdx];

            for(unsigned int k = 0; k < matrixVector.size(); ++k)
            {
                const auto matrix = matrixVector[k].lock();
                OSLM_ASSERT("Matrix with indices '"<< tlIdx << ", " << k << "' does not exist", matrix);

                const int sendStatus = m_sendMatricesStatus[tlIdx][k];

                if(buffer->isPresent(k))
                {
                    const auto& values = buffer->getElement(k);
                    for(std::uint8_t i = 0; i < 4; ++i)
                    {
                        for(std::uint8_t j = 0; j < 4; ++j)
                        {
                            matrix->setCoefficient(i, j, static_cast<double>(values[i * 4 + j]));
                        }
                    }

                    if(sendStatus != -1)
                    {
                        m_sigMatrixSynchronized->asyncEmit(sendStatus);
                    }

                    auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(
                        ::fwData::Object::s_MODIFIED_SIG);
                    sig->asyncEmit();

                    matrixFound = true;
                    ++syncMatricesNbr;
                }
                else if(sendStatus != -1)
                {
                    m_sigMatrixUnsynchronized->asyncEmit(sendStatus);
                }

            }
        }
        else
        {
            for(const int sendStatus : m_sendMatricesStatus[tlIdx])
            {
                if(sendStatus != -1)
                {
                    m_sigMatrixUnsynchronized->asyncEmit(sendStatus);
                }
            }
        }
    }

    m_updateMask = m_timeStep != 0 ? OBJECT_RECEIVED : 0;

    if (matrixFound)
    {
        m_sigSynchronizationDone->asyncEmit(matrixTimestamp);
    }
    m_sigAllMatricesFound->asyncEmit(m_totalOutputMatrices == syncMatricesNbr);
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::updating()
{
    m_updateMask |= OBJECT_RECEIVED;

    if(m_updateMask & SYNC_REQUESTED)
    {
        this->synchronize();
    }
}

//-----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::setFrameDelay(int val, std::string key)
{
    if(key == "frameDelay")
    {
        m_delay = val;
    }
    else
    {
        OSLM_WARN("Unknown key");
    }
}

//-----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::resetTimeline()
{
    // Reset the last timestamp in case the grabber uses a different time scale
    m_lastTimestamp = std::numeric_limits<double>::lowest();
}

// ----------------------------------------------------------------------------

}  // namespace videoTools

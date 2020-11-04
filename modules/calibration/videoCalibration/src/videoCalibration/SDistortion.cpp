/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "videoCalibration/SDistortion.hpp"

#include <arData/Camera.hpp>

#include <cvIO/Camera.hpp>
#include <cvIO/Image.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#define FW_PROFILING_DISABLED
#include <fwCore/Profiling.hpp>

#include <fwData/Array.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <opencv2/imgproc.hpp>

namespace videoCalibration
{

// Public slot
const ::fwCom::Slots::SlotKeyType SDistortion::s_CHANGE_STATE_SLOT = "changeState";

// Private slot
static const ::fwCom::Slots::SlotKeyType s_CALIBRATE_SLOT = "calibrate";

const ::fwServices::IService::KeyType s_CAMERA_INPUT = "camera";
const ::fwServices::IService::KeyType s_IMAGE_INPUT  = "input";
const ::fwServices::IService::KeyType s_IMAGE_INOUT  = "output";
const ::fwServices::IService::KeyType s_MAP_INOUT    = "map";

//------------------------------------------------------------------------------
SDistortion::SDistortion() noexcept :
    m_isEnabled(false)
{
    newSlot(s_CHANGE_STATE_SLOT, &SDistortion::changeState, this);
    newSlot(s_CALIBRATE_SLOT, &SDistortion::calibrate, this);
}

//------------------------------------------------------------------------------

SDistortion::~SDistortion() noexcept
{
}

// ----------------------------------------------------------------------------

fwServices::IService::KeyConnectionsMap SDistortion::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_CAMERA_INPUT, ::arData::Camera::s_MODIFIED_SIG, s_CALIBRATE_SLOT);
    connections.push(s_CAMERA_INPUT, ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG, s_CALIBRATE_SLOT);
    connections.push( s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_IMAGE_INPUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

void SDistortion::configuring()
{
    const auto config = this->getConfigTree();

    const auto mode = config.get<std::string>("mode");
    if(mode == "undistort")
    {
        m_distort = false;
    }
    else if(mode != "distort")
    {
        SLM_ERROR("Mode should be distort or undistort");
    }
}

//------------------------------------------------------------------------------

void SDistortion::starting()
{
    this->calibrate();
}

//------------------------------------------------------------------------------

void SDistortion::stopping()
{
    m_calibrationMismatch = false;
    m_prevImageSize       = {0, 0, 0};
}

//------------------------------------------------------------------------------

void SDistortion::updating()
{
    ::fwData::Image::csptr inputImage = this->getInput< ::fwData::Image> ( s_IMAGE_INPUT);
    if(inputImage && m_calibrationMismatch)
    {
        const ::fwData::mt::ObjectReadLock inputLock(inputImage);
        const auto inputSize = inputImage->getSize2();
        if(inputSize != m_prevImageSize)
        {
            // Reset the error detection boolean
            m_calibrationMismatch = false;
            m_prevImageSize       = {0, 0, 0};
        }
    }

    if (m_isEnabled)
    {
        ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
        SLM_FATAL_IF("Object key '" + s_CAMERA_INPUT + "' is not found.", !camera);
        if (camera->getIsCalibrated())
        {
            this->remap();
        }
        else
        {
            SLM_WARN("Unable to distort/undistort the image: camera '" + camera->getID() + "' is not calibrated.");
        }
    }
    else
    {
        // Simple copy of the input image
        ::fwData::Image::csptr inputImage = this->getInput< ::fwData::Image> ( s_IMAGE_INPUT);
        ::fwData::Image::sptr outputImage = this->getInOut< ::fwData::Image >( s_IMAGE_INOUT);

        if(inputImage && outputImage )
        {
            ::fwData::mt::ObjectReadLock inputLock(inputImage);
            ::fwData::mt::ObjectWriteLock outputLock(outputImage);

            // Since we shallow copy the input image when no remap is done,
            // we have to notify the output image pointer has changed if it was not shared yet before
            bool reallocated = false;
            {
                const auto inDumpLock  = inputImage->lock();
                const auto outDumpLock = outputImage->lock();
                reallocated = inputImage->getBuffer() != outputImage->getBuffer();
            }

            // Shallow copy the image is faster
            // We only have to take care about reallocating a new buffer when we perform the distortion
            outputImage->shallowCopy(inputImage);

            if(reallocated)
            {
                auto sig =
                    outputImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
                {
                    ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                    sig->asyncEmit();
                }
            }

            auto sig = outputImage->signal< ::fwData::Image::BufferModifiedSignalType >(
                ::fwData::Image::s_BUFFER_MODIFIED_SIG);
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }
    }
}

//------------------------------------------------------------------------------

void SDistortion::remap()
{
    ::fwData::Image::csptr inputImage = this->getInput< ::fwData::Image> ( s_IMAGE_INPUT);
    ::fwData::Image::sptr outputImage = this->getInOut< ::fwData::Image >( s_IMAGE_INOUT);

    if(!inputImage || !outputImage || m_calibrationMismatch)
    {
        return;
    }
    FW_PROFILE_AVG("distort", 5);

    auto sig = inputImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Image::s_BUFFER_MODIFIED_SIG);
    // Blocking signals early allows to discard any event while we are updating
    ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));

    ::fwData::mt::ObjectReadLock inputLock(inputImage);

    const auto inputSize = inputImage->getSize2();

    if (inputImage->getSizeInBytes() == 0 || inputImage->getNumberOfDimensions() < 2)
    {
        SLM_WARN("Can not remap this image, it is empty.");
        return;
    }

    ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT);
    SLM_ASSERT("Object key '" + s_CAMERA_INPUT + "' is not found.", camera);
    if(inputSize[0] != camera->getWidth() || inputSize[1] != camera->getHeight())
    {
        std::stringstream msg;
        msg << "Can not distort/undistort, the camera calibration resolution [" <<
            camera->getWidth() << "x" << camera->getHeight() << "] does not match the input image size [" <<
            inputSize[0] << "x" << inputSize[1] << "]";

        ::fwGui::dialog::MessageDialog::show("Error", msg.str(),
                                             ::fwGui::dialog::IMessageDialog::CRITICAL);

        m_calibrationMismatch = true;
        m_prevImageSize       = inputSize;
        return;
    }

    const auto prevSize = outputImage->getSize2();
    const auto dumpLock = inputImage->lock();
    // Since we shallow copy the input image when no remap is done
    // We have to reallocate the output image if it still shares the buffer
    bool realloc = false;
    {
        const auto outDumpLock = outputImage->lock();
        realloc = inputImage->getBuffer() == outputImage->getBuffer();
    }
    if(prevSize != inputSize || realloc)
    {
        ::fwData::mt::ObjectWriteLock outputLock(outputImage);
        ::fwData::Image::Size size = {inputSize[0], inputSize[1], 0};

        // Since we may have shared the pointer on the input image, we can't use ::fwData::Image::allocate
        // Because it will not give us a new buffer and will thus make us modify both input and output images
        ::fwData::Image::sptr tmpImage = ::fwData::Image::New();
        outputImage->shallowCopy(tmpImage);
        outputImage->resize(size, inputImage->getType(), inputImage->getPixelFormat());

        const ::fwData::Image::Origin origin = {0., 0., 0.};
        outputImage->setOrigin2(origin);

        const ::fwData::Image::Spacing spacing = {1., 1., 1.};
        outputImage->setSpacing2(spacing);
        outputImage->setWindowWidth(1);
        outputImage->setWindowCenter(0);
        if (inputImage->getNumberOfComponents() != outputImage->getNumberOfComponents())
        {
            FW_DEPRECATED_MSG("Number of components should be defined by pixel format", "sight 22.0");
            outputImage->setNumberOfComponents(inputImage->getNumberOfComponents());
            outputImage->resize();
        }
    }
    const auto newSize = outputImage->getSize2();

    // Get ::cv::Mat from fwData::Image
    auto inImage = std::const_pointer_cast< ::fwData::Image>(inputImage);
    ::cv::Mat img = ::cvIO::Image::moveToCv(inImage);

    ::cv::Mat undistortedImage;

#ifndef OPENCV_CUDA_SUPPORT
    ::fwData::mt::ObjectWriteLock outputLock(outputImage);
    if(outputImage != inputImage)
    {
        undistortedImage = ::cvIO::Image::moveToCv(outputImage);
    }
#endif

    {
#ifdef OPENCV_CUDA_SUPPORT
        FW_PROFILE_AVG("cv::cuda::remap", 5);

        ::cv::cuda::GpuMat image_gpu(img);
        ::cv::cuda::GpuMat image_gpu_rect(undistortedImage);
        ::cv::cuda::remap(image_gpu, image_gpu_rect, m_mapx, m_mapy, ::cv::INTER_LINEAR, ::cv::BORDER_CONSTANT);
        undistortedImage = ::cv::Mat(image_gpu_rect);

        ::fwData::mt::ObjectWriteLock outputLock(outputImage);
        ::cvIO::Image::copyFromCv(outputImage, undistortedImage);

#else
        FW_PROFILE_AVG("cv::remap", 5);

        ::cv::remap(img, undistortedImage, m_mapx, m_mapy, ::cv::INTER_LINEAR, ::cv::BORDER_CONSTANT);

        const auto outDumpLock = outputImage->lock();
        if(outputImage == inputImage)
        {
            // Copy new image.
            // According to OpenCv's doc, if img and undistortedImage have
            // the same size and type, no reallocation will be done. i.e:
            // this call should copy the undistorted image to the video's
            // frameBuffer.
            undistortedImage.copyTo(img);
            SLM_ASSERT("OpenCV did something wrong.", img.data == inputImage->getBuffer());
        }
        else
        {
            SLM_ASSERT("OpenCV did something wrong.", undistortedImage.data == outputImage->getBuffer());
        }
#endif // OPENCV_CUDA_SUPPORT
    }

    if(prevSize != newSize)
    {
        auto sigModified = outputImage->signal< ::fwData::Image::ModifiedSignalType >(::fwData::Image::s_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sigModified->getConnection(m_slotUpdate));
            sigModified->asyncEmit();
        }
    }

    auto sigOut = outputImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Image::s_BUFFER_MODIFIED_SIG);
    sigOut->asyncEmit();
}

// ----------------------------------------------------------------------------

void SDistortion::changeState()
{
    // Reset the error detection boolean
    m_calibrationMismatch = false;
    m_prevImageSize       = {0, 0, 0};

    m_isEnabled = !m_isEnabled;
}

// ----------------------------------------------------------------------------

void SDistortion::calibrate()
{
    // Reset the error detection boolean
    m_calibrationMismatch = false;
    m_prevImageSize       = {0, 0, 0};

    auto camera = this->getInput< ::arData::Camera> (s_CAMERA_INPUT);
    SLM_FATAL_IF("Object 'camera' is not found.", !camera);

    ::cv::Mat intrinsics;
    ::cv::Mat distCoefs;
    ::cv::Size size;

    std::tie(intrinsics, size, distCoefs) = ::cvIO::Camera::copyToCv(camera);

    std::vector< ::cv::Mat> xyMaps(2);

    if(m_distort)
    {
        ::cv::Mat pixel_locations_src      = ::cv::Mat( size, CV_32FC2);
        ::cv::Mat fractional_locations_dst = ::cv::Mat( size, CV_32FC2);

        for (int i = 0; i < size.height; i++)
        {
            for (int j = 0; j < size.width; j++)
            {
                pixel_locations_src.at< ::cv::Point2f>(i, j) = ::cv::Point2f(j, i);
            }
            ::cv::undistortPoints(pixel_locations_src.row(i),
                                  fractional_locations_dst.row(i),
                                  intrinsics, distCoefs);
        }

        ::cv::Mat pixelLocations = ::cv::Mat( size, CV_32FC2);

        // Output from undistortPoints is normalized point coordinates
        const float fx = static_cast<float>(intrinsics.at<double>(0, 0));
        const float fy = static_cast<float>(intrinsics.at<double>(1, 1));
        const float cx = static_cast<float>(intrinsics.at<double>(0, 2));
        const float cy = static_cast<float>(intrinsics.at<double>(1, 2));

        for (int i = 0; i < size.height; i++)
        {
            for (int j = 0; j < size.width; j++)
            {
                const float x = fractional_locations_dst.at< ::cv::Point2f>(i, j).x*fx + cx;
                const float y = fractional_locations_dst.at< ::cv::Point2f>(i, j).y*fy + cy;
                pixelLocations.at< ::cv::Point2f>(i, j) = ::cv::Point2f(x, y);
            }
        }

        ::cv::split(pixelLocations, xyMaps);
    }
    else
    {
        ::cv::initUndistortRectifyMap(intrinsics, distCoefs, ::cv::Mat(), intrinsics, size, CV_32FC1,
                                      xyMaps[0], xyMaps[1]);
    }

    ::fwData::Image::sptr map = this->getInOut< ::fwData::Image >( s_MAP_INOUT);
    if(map)
    {
        ::cv::Mat cvMap;
        ::cv::merge(xyMaps, cvMap);

        ::fwData::mt::ObjectWriteLock outputLock(map);
        ::cvIO::Image::copyFromCv(map, cvMap);

        auto sigModified = map->signal< ::fwData::Image::ModifiedSignalType >(::fwData::Image::s_MODIFIED_SIG);
        sigModified->asyncEmit();
    }
    else
    {
#if OPENCV_CUDA_SUPPORT
        m_mapx = ::cv::cuda::GpuMat(xyMaps[0]);
        m_mapy = ::cv::cuda::GpuMat(xyMaps[1]);
#else
        m_mapx = xyMaps[0];
        m_mapy = xyMaps[1];
#endif // OPENCV_CUDA_SUPPORT
    }
}

//------------------------------------------------------------------------------

} // namespace videoCalibration

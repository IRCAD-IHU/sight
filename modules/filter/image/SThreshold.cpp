/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/filter/image/SThreshold.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/tools/Dispatcher.hpp>
#include <core/tools/fwID.hpp>
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>

#include <service/macros.hpp>

namespace sight::module::filter::image
{

//-----------------------------------------------------------------------------

static const std::string s_IMAGE_INPUT  = "source";
static const std::string s_IMAGE_OUTPUT = "target";

//-----------------------------------------------------------------------------

SThreshold::SThreshold() noexcept :
    m_threshold(50.0)
{
}

//-----------------------------------------------------------------------------

SThreshold::~SThreshold() noexcept
{
}

//-----------------------------------------------------------------------------

void SThreshold::starting()
{
}

//-----------------------------------------------------------------------------

void SThreshold::stopping()
{
}

//-----------------------------------------------------------------------------

void SThreshold::configuring()
{
    const service::IService::ConfigType& srvConfig = this->getConfigTree();

    SIGHT_ASSERT("You must have one <config/> element.", srvConfig.count("config") == 1);

    const service::IService::ConfigType& config = srvConfig.get_child("config");

    SIGHT_ASSERT("You must have one <threshold/> element.", config.count("threshold") == 1);
    const service::IService::ConfigType& thresholdCfg = config.get_child("threshold");
    m_threshold = thresholdCfg.get_value<double>();
}

//-----------------------------------------------------------------------------

/**
 * Functor to apply a threshold filter.
 *
 * The pixel with the value less than the threshold value will be set to 0, else the value is set to the maximum
 * value of the image pixel type.
 *
 * The functor provides a template method operator(param) to apply the filter
 */
struct ThresholdFilter
{
    struct Parameter
    {
        double thresholdValue;      ///< threshold value.
        data::Image::csptr imageIn; ///< image source
        data::Image::sptr imageOut; ///< image target: contains the result of the filter
    };

    /**
     * @brief Applies the filter
     * @tparam PIXELTYPE image pixel type (uint8, uint16, int8, int16, float, double, ....)
     */
    template<class PIXELTYPE>
    void operator()(Parameter& param)
    {
        const PIXELTYPE thresholdValue = static_cast<PIXELTYPE>(param.thresholdValue);
        data::Image::csptr imageIn     = param.imageIn;
        data::Image::sptr imageOut     = param.imageOut;
        SIGHT_ASSERT("Sorry, image must be 3D", imageIn->getNumberOfDimensions() == 3);

        imageOut->copyInformation(imageIn); // Copy image size, type... without copying the buffer
        imageOut->resize();                 // Allocate the image buffer

        // Get iterators on image buffers
        auto it1          = imageIn->begin<PIXELTYPE>();
        const auto it1End = imageIn->end<PIXELTYPE>();
        auto it2          = imageOut->begin<PIXELTYPE>();
        const auto it2End = imageOut->end<PIXELTYPE>();

        const PIXELTYPE maxValue = std::numeric_limits<PIXELTYPE>::max();

        // Fill the target buffer considering the thresholding
        for( ; it1 != it1End && it2 != it2End ; ++it1, ++it2)
        {
            *it2 = (*it1 < thresholdValue) ? 0 : maxValue;
        }
    }
};

//-----------------------------------------------------------------------------

void SThreshold::updating()
{
    ThresholdFilter::Parameter param; // filter parameters: threshold value, image source, image target

    // retrieve the input object
    auto input = this->getLockedInput<data::Object>(s_IMAGE_INPUT);

    // try to dynamic cast to an Image and an ImageSeries to know whick type of data we use
    data::ImageSeries::csptr imageSeriesSrc = data::ImageSeries::dynamicConstCast(input.get_shared());
    data::Image::csptr imageSrc             = data::Image::dynamicConstCast(input.get_shared());
    data::Object::sptr output;

    // Get source/target image
    if(imageSeriesSrc)
    {
        param.imageIn = imageSeriesSrc->getImage();
        data::ImageSeries::sptr imageSeriesDest = data::ImageSeries::copy(imageSeriesSrc);
        // define the input image series as the reference
        imageSeriesDest->setDicomReference(imageSeriesSrc->getDicomReference());

        // create the output image
        data::Image::sptr imageOut = data::Image::New();
        imageSeriesDest->setImage(imageOut);
        param.imageOut = imageOut;
        output         = imageSeriesDest;
    }
    else if(imageSrc)
    {
        param.imageIn = imageSrc;
        // create the output image
        data::Image::sptr imageOut = data::Image::New();
        param.imageOut = imageOut;
        output         = imageOut;
    }
    else
    {
        SIGHT_THROW("Wrong type: source type must be an ImageSeries or an Image");
    }

    param.thresholdValue = m_threshold;

    // get image type
    core::tools::Type type = param.imageIn->getType();

    /* The dispatcher allows to apply the filter on any type of image.
     * It invokes the template functor ThresholdFilter using the image type.
     * - template parameters:
     *   - core::tools::SupportedDispatcherTypes defined all the supported type of the functor, here all the type
     *     supportted by core::tools::Type(std::int8_t, std::uint8_t, std::int16_t, std::uint16_t, std::int32_t,
     *     std::uint32_t, std::int64_t, std::uint64_t, float, double)
     *   - ThresholdFilter: functor struct or class
     * - parameters:
     *   - type: core::tools::Type of the image
     *   - param: struct containing the functor parameters (here the input and output images and the threshold value)
     */
    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, ThresholdFilter>::invoke(type, param);

    // register the output image to be accesible by the other service from the XML configuration
    this->setOutput(s_IMAGE_OUTPUT, output);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::image

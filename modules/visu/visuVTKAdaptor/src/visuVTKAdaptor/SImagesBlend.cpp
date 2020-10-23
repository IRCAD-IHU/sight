/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "visuVTKAdaptor/SImagesBlend.hpp"

#include "visuVTKAdaptor/SImage.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Color.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/String.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwMath/Compare.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <fwVtkIO/vtk.hpp>

#include <boost/foreach.hpp>

#include <vtkImageBlend.h>
#include <vtkImageCheckerboard.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>

#include <regex>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SImagesBlend, ::fwData::Composite )

namespace visuVTKAdaptor
{

const ::fwCom::Slots::SlotKeyType s_CHANGE_MODE_SLOT                  = "changeMode";
const ::fwCom::Slots::SlotKeyType s_CHANGE_CHECKERBOARD_DIVISION_SLOT = "changeCheckerboardDivision";
const ::fwCom::Slots::SlotKeyType s_SET_TOP_IMAGE_TRANSPARENCY_SLOT   = "setTopImageTransparency";

static const ::fwServices::IService::KeyType s_IMAGE_GROUP = "image";
static const ::fwServices::IService::KeyType s_TF_GROUP    = "tf";

//------------------------------------------------------------------------------

SImagesBlend::SImagesBlend() noexcept :
    m_imageAlgorithm(nullptr),
    m_checkerboardDivision(10),
    m_zDivision(m_checkerboardDivision)
{
    newSlot(s_CHANGE_MODE_SLOT, &SImagesBlend::changeMode, this);
    newSlot(s_CHANGE_CHECKERBOARD_DIVISION_SLOT, &SImagesBlend::changeCheckerboardDivision, this);
    newSlot(s_SET_TOP_IMAGE_TRANSPARENCY_SLOT, &SImagesBlend::setTopImageTransparency, this);
}

//------------------------------------------------------------------------------

SImagesBlend::~SImagesBlend() noexcept
{
    m_imageAlgorithm = nullptr;
}

//------------------------------------------------------------------------------

void SImagesBlend::starting()
{
    this->initialize();

    SLM_ASSERT("Image register is empty", !m_imageRegisterId.empty());

    // Try to downcast as an vtkImageBlend first
    m_imageAlgorithm = vtkImageBlend::SafeDownCast(this->getVtkObject(m_imageRegisterId));

    if(nullptr == m_imageAlgorithm)
    {
        // If we have no vtkImageBlend, try to downcast as an vtkImageCheckerboard
        m_imageAlgorithm = vtkImageCheckerboard::SafeDownCast(this->getVtkObject(m_imageRegisterId));
    }

    // If we have a null m_imageAlgorithm, then we have a problem Houston
    SLM_ASSERT("Stored image algorithm doesn't contain a vtkImageBlend or vtkImageCheckerboard",
               nullptr != m_imageAlgorithm);

    this->addImageAdaptors();

    // Set the divisions once all image info has been gathered.
    this->changeCheckerboardDivision(m_checkerboardDivision);
}

//------------------------------------------------------------------------------

void SImagesBlend::stopping()
{
    this->removeImageAdaptors();
    m_imageAlgorithm = nullptr;
    m_imagesInfo.clear();
}

//------------------------------------------------------------------------------

void SImagesBlend::updating()
{
    this->removeImageAdaptors();
    this->addImageAdaptors();
}

//------------------------------------------------------------------------------

void SImagesBlend::configuring()
{
    this->configureParams();

    const ConfigType srvConfig = this->getConfigTree();

    BOOST_FOREACH(const ::fwServices::IService::ConfigType::value_type& cfg, srvConfig.equal_range("inout"))
    {
        const ConfigType inoutConfig = cfg.second;
        const std::string group      = inoutConfig.get<std::string>("<xmlattr>.group");
        if (group == s_IMAGE_GROUP)
        {
            BOOST_FOREACH(const ::fwServices::IService::ConfigType::value_type& v, inoutConfig.equal_range("key"))
            {
                const ::fwServices::IService::ConfigType& specAssoc = v.second;
                const ::fwServices::IService::ConfigType& attr      = specAssoc.get_child("<xmlattr>");
                const std::string tfalpha                           = attr.get("tfalpha", "no");
                const double opacity                                = attr.get("opacity", 1.0);

                ImageInfo info;
                info.m_imageOpacity = opacity;
                info.m_useTFAlfa    = (tfalpha == "yes");
                m_imagesInfo.push_back(info);
            }
        }
    }

    const ConfigType config = srvConfig.get_child("config.<xmlattr>");

    const std::string vtkimageregister = config.get("vtkimageregister", "");
    SLM_ASSERT("'vtkimageregister' must be defined", !vtkimageregister.empty());
    this->setVtkImageRegisterId(vtkimageregister);

    // Get the default division count for checkerboard algorithm
    m_checkerboardDivision = config.get<int>("checkerboardDivision", 10);
}

//------------------------------------------------------------------------------

void SImagesBlend::swapping(const KeyType& key)
{
    const std::string regexStr = "([\\w]+)#([\\d]+)";
    const std::regex re(regexStr);
    std::smatch match;

    if( std::regex_match(key, match, re) )
    {
        const std::string group   = match[1];
        const unsigned long index = std::stoul(match[2]);

        if (group == s_TF_GROUP && this->getRegisteredServices().size() > index)
        {
            ::fwServices::IService::wptr wsrv = this->getRegisteredServices()[index];

            if (!wsrv.expired())
            {
                const ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_GROUP,
                                                                                                        index);
                ::fwServices::IService::sptr service = wsrv.lock();
                if(tf != nullptr)
                {
                    service->registerInOut(tf, SImage::s_TF_INOUT, false, true);
                    service->swapKey(SImage::s_TF_INOUT, nullptr);
                }
                else if(::fwServices::OSR::isRegistered(SImage::s_TF_INOUT, AccessType::INOUT, service))
                {
                    service->unregisterInOut(SImage::s_TF_INOUT);
                    service->swapKey(SImage::s_TF_INOUT, nullptr);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

bool SImagesBlend::checkImageInformations()
{
    ::fwData::Image::Size size;
    ::fwData::Image::Spacing spacing;
    ::fwData::Image::Origin origin;

    bool haveSameInfo = true;
    bool firstImage   = true;

    const size_t nbImages = this->getKeyGroupSize(s_IMAGE_GROUP);

    for(size_t i = 0; i < nbImages; ++i)
    {
        ::fwData::Image::csptr img = this->getInOut< ::fwData::Image >(s_IMAGE_GROUP, i);

        if (img && ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( img ))
        {
            if (firstImage)
            {
                size    = img->getSize2();
                spacing = img->getSpacing2();
                origin  = img->getOrigin2();

                if(size.size() < 3 || size[2] <= 1)
                {
                    m_zDivision = 1;
                }
                firstImage = false;
            }
            else
            {
                if (  size != img->getSize2() ||
                      !::fwMath::isContainerEqual< const ::fwData::Image::Spacing >(spacing,
                                                                                    img->getSpacing2()) ||
                      !::fwMath::isContainerEqual< const ::fwData::Image::Origin >(origin,
                                                                                   img->getOrigin2()) )
                {
                    SLM_ERROR("imgA size : " << size[0] << " / " << size[1] << " / "<< size[2] );
                    SLM_ERROR("imgA spacing : " << spacing[0] << " / " << spacing[1] << " / "<< spacing[2] );
                    SLM_ERROR("imgA origin : " << origin[0] << " / " << origin[1] << " / "<< origin[2] );

                    SLM_ERROR(
                        "imgB size : " << img->getSize2()[0] << " / " << img->getSize2()[1] << " / "<<
                            img->getSize2()[2] );
                    SLM_ERROR(
                        "imgB spacing : " << img->getSpacing2()[0] << " / " << img->getSpacing2()[1] << " / "<<
                            img->getSpacing2()[2] );
                    SLM_ERROR(
                        "imgB origin : " << img->getOrigin2()[0] << " / " << img->getOrigin2()[1] << " / "<<
                            img->getOrigin2()[2] );

                    haveSameInfo = false;
                    std::string errorMsg = "Warning : images in blend have not the same";
                    errorMsg += (size != img->getSize2()) ? " size" : "";
                    errorMsg += (spacing != img->getSpacing2()) ? " spacing" : "";
                    errorMsg += (origin != img->getOrigin2()) ? " origin" : "";
                    errorMsg += ".\n Background image size, spacing and origin are use.";
                    ::fwGui::dialog::MessageDialog::showMessageDialog("Images blending",
                                                                      errorMsg,
                                                                      ::fwGui::dialog::IMessageDialog::WARNING);
                    break;
                }
            }
        }
    }

    return haveSameInfo;
}

//------------------------------------------------------------------------------

void SImagesBlend::addImageAdaptors()
{
    this->checkImageInformations();

    int addedImageCount   = 0;
    size_t lastValidIndex = 0;

    const size_t nbImages = this->getKeyGroupSize(s_IMAGE_GROUP);
    const size_t nbTFs    = this->getKeyGroupSize(s_TF_GROUP);
    SLM_ASSERT("'" + s_TF_GROUP + "' group must have the same number of elements that '" + s_IMAGE_GROUP +"'",
               nbTFs == 0 || nbImages == nbTFs);

    for(size_t i = 0; i < nbImages; ++i)
    {
        ::fwData::Image::sptr img = this->getInOut< ::fwData::Image >(s_IMAGE_GROUP, i);
        ::fwData::TransferFunction::sptr tf;

        if (img)
        {
            if (nbTFs > 0)
            {
                tf = this->getInOut< ::fwData::TransferFunction >(s_TF_GROUP, i);
            }

            const ImageInfo& info = m_imagesInfo[i];

            bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( img );
            if (imageIsValid)
            {
                this->addImage(img, tf, info);

                ++addedImageCount;
                lastValidIndex = i;
            }
        }
    }

    // If Checkerboard is used and only one image is valid, we must duplicate the image adaptor to display the image
    if(addedImageCount == 1 && nullptr != vtkImageCheckerboard::SafeDownCast(this->getVtkObject(m_imageRegisterId)))
    {
        ::fwData::Image::sptr img = this->getInOut< ::fwData::Image >(s_IMAGE_GROUP, lastValidIndex);
        ::fwData::TransferFunction::sptr tf;
        if (nbTFs > 0)
        {
            tf = this->getInOut< ::fwData::TransferFunction >(s_TF_GROUP, lastValidIndex);
        }
        const ImageInfo& info = m_imagesInfo[lastValidIndex];
        this->addImage(img, tf, info);
    }
}

//------------------------------------------------------------------------------

void SImagesBlend::removeImageAdaptors()
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImagesBlend::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_GROUP, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_GROUP, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SImagesBlend::changeMode(std::string _value, std::string _key)
{
    if( _key == "ImageSource" )
    {
        // Select the right algorithm
        m_imageRegisterId = _value;

        // Try to downcast as an vtkImageBlend first
        m_imageAlgorithm = vtkImageBlend::SafeDownCast(this->getVtkObject(m_imageRegisterId));

        if(nullptr == m_imageAlgorithm)
        {
            // If we have no vtkImageBlend, try to downcast as an vtkImageCheckerboard
            vtkImageCheckerboard* imageCheckerboard =
                vtkImageCheckerboard::SafeDownCast(this->getVtkObject(m_imageRegisterId));

            if(nullptr != imageCheckerboard)
            {
                // Set the number of subdivision
                imageCheckerboard->SetNumberOfDivisions(m_checkerboardDivision, m_checkerboardDivision,
                                                        m_zDivision);

                // Assign as an vtkThreadedImageAlgorithm
                m_imageAlgorithm = imageCheckerboard;
            }
        }

        // Update
        this->updating();
    }
}

//------------------------------------------------------------------------------

void SImagesBlend::addImage(::fwData::Image::sptr img, ::fwData::TransferFunction::sptr tf, const ImageInfo& info)
{
    // create the srv configuration for objects auto-connection
    auto imageAdaptor = this->registerService< ::visuVTKAdaptor::SImage>("::visuVTKAdaptor::SImage");
    // register image
    imageAdaptor->registerInOut(img, SImage::s_IMAGE_INOUT, true);
    if (tf)
    {
        imageAdaptor->registerInOut(tf, SImage::s_TF_INOUT, false, true);
    }

    imageAdaptor->setRenderService(this->getRenderService());
    imageAdaptor->setRendererId( this->getRendererId() );
    imageAdaptor->setPickerId( this->getPickerId() );
    imageAdaptor->setTransformId( this->getTransformId() );

    imageAdaptor->setVtkImageRegister(m_imageAlgorithm);
    imageAdaptor->setImageOpacity(info.m_imageOpacity);
    imageAdaptor->setAllowAlphaInTF(info.m_useTFAlfa);

    imageAdaptor->start();
}

//------------------------------------------------------------------------------

void SImagesBlend::setTopImageTransparency(int transparency)
{
    SLM_ASSERT("The image group is empty. Have you started the service?", m_imagesInfo.size() > 0);
    SLM_ERROR_IF("Transparency must be in [0, 255].", transparency < 0 || transparency > 255);
    auto topImageInfo = m_imagesInfo.rbegin();

    topImageInfo->m_imageOpacity = 1. - (double(transparency) / 255.);
    this->updating();
}

//------------------------------------------------------------------------------

void SImagesBlend::changeCheckerboardDivision(const int division)
{
    m_checkerboardDivision = division;

    // If we have no vtkImageBlend, try to downcast as an vtkImageCheckerboard
    vtkImageCheckerboard* imageCheckerboard =
        vtkImageCheckerboard::SafeDownCast(this->getVtkObject(m_imageRegisterId));

    if(nullptr != imageCheckerboard)
    {
        // Set the number of subdivision
        imageCheckerboard->SetNumberOfDivisions(m_checkerboardDivision, m_checkerboardDivision,
                                                m_zDivision);

        // Assign as an vtkThreadedImageAlgorithm
        m_imageAlgorithm = imageCheckerboard;
    }

    this->updating();
}

} //namespace visuVTKAdaptor

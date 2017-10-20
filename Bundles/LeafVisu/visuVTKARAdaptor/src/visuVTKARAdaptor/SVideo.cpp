/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SVideo.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/helper/TransferFunction.hpp>
#include <fwVtkIO/vtk.hpp>

#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMapToColors.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKARAdaptor::SVideo);

namespace visuVTKARAdaptor
{

static const ::fwServices::IService::KeyType s_IMAGE_INPUT = "frame";
static const ::fwServices::IService::KeyType s_TF_INPUT    = "tf";

static const ::fwCom::Slots::SlotKeyType s_UPDATE_IMAGE_SLOT         = "updateImage";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_IMAGE_OPACITY_SLOT = "updateImageOpacity";
static const ::fwCom::Slots::SlotKeyType s_SHOW_SLOT                 = "show";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF_SLOT            = "updateTF";

//------------------------------------------------------------------------------

SVideo::SVideo() noexcept :
    m_imageData(vtkSmartPointer<vtkImageData>::New()),
    m_actor(vtkSmartPointer<vtkImageActor>::New()),
    m_lookupTable(vtkSmartPointer<vtkLookupTable>::New()),
    m_isTextureInit(false),
    m_reverse(true),
    m_interpolate(true)
{
    newSlot(s_UPDATE_IMAGE_SLOT, &SVideo::updateImage, this);
    newSlot(s_UPDATE_IMAGE_OPACITY_SLOT, &SVideo::updateImageOpacity, this);
    newSlot(s_SHOW_SLOT, &SVideo::show, this);
    newSlot(s_UPDATE_TF_SLOT, &SVideo::updateTF, this);
}

//------------------------------------------------------------------------------

SVideo::~SVideo() noexcept
{
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SVideo::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_IMAGE_SLOT);
    connections.push( s_IMAGE_INPUT, ::fwData::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_IMAGE_OPACITY_SLOT);
    connections.push( s_IMAGE_INPUT, ::fwData::Image::s_TRANSPARENCY_MODIFIED_SIG, s_UPDATE_IMAGE_OPACITY_SLOT);
    connections.push( s_IMAGE_INPUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    connections.push( s_TF_INPUT, ::fwData::TransferFunction::s_POINTS_MODIFIED_SIG, s_UPDATE_TF_SLOT);
    connections.push( s_TF_INPUT, ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG, s_UPDATE_TF_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void SVideo::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_reverse     = config.get<bool>("reverse", m_reverse);
    m_interpolate = config.get<bool>("interpolate", m_interpolate);
}

//------------------------------------------------------------------------------

void SVideo::starting()
{
    this->initialize();

    if (m_reverse)
    {
        m_actor->RotateZ(180);
        m_actor->RotateY(180);
    }

    ::fwData::TransferFunction::csptr tf = this->getInput< ::fwData::TransferFunction>(s_TF_INPUT);
    if(tf)
    {
        this->updateTF();
    }

    this->updating();

    if (this->getPicker())
    {
        this->addToPicker(m_actor);
    }
}

//------------------------------------------------------------------------------

void SVideo::updating()
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_INPUT);
    const bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (!imageIsValid)
    {
        return;
    }

    ::fwVtkIO::toVTKImage(image, m_imageData);

    if(!m_isTextureInit)
    {
        this->removeAllPropFromRenderer();

        // Reset the transform matrix first
        m_actor->PokeMatrix(vtkMatrix4x4::New());
        if (m_reverse)
        {
            m_actor->RotateZ(180);
            m_actor->RotateY(180);
        }

        const ::fwData::Image::SizeType size = image->getSize();

        if(this->getInput< ::fwData::TransferFunction>(s_TF_INPUT))
        {
            auto scalarValuesToColors = vtkSmartPointer<vtkImageMapToColors>::New();
            scalarValuesToColors->SetLookupTable(m_lookupTable);
            scalarValuesToColors->PassAlphaToOutputOn();
            scalarValuesToColors->SetInputData(m_imageData);
            m_actor->GetMapper()->SetInputConnection(scalarValuesToColors->GetOutputPort());
        }
        else
        {
            m_actor->SetInputData(m_imageData);
        }
        this->addToRenderer(m_actor);

        m_actor->SetInterpolate(m_interpolate);

        m_isTextureInit = true;

        this->getRenderer()->InteractiveOff();
        this->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
        this->getRenderer()->ResetCamera();
        this->getRenderer()->GetActiveCamera()->SetParallelScale(static_cast<double>(size[1]) / 2.0);
    }

    m_imageData->Modified();
    this->setVtkPipelineModified();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SVideo::swapping(const KeyType& key)
{
    if (key == s_TF_INPUT)
    {
        ::fwData::TransferFunction::csptr tf = this->getInput< ::fwData::TransferFunction>(s_TF_INPUT);
        if (tf)
        {
            this->updateTF();
        }
        this->updating();
    }
}

//------------------------------------------------------------------------------

void SVideo::stopping()
{
    this->unregisterServices();
    this->removeAllPropFromRenderer();
    if (this->getPicker())
    {
        this->removeFromPicker(m_actor);
    }
}

//------------------------------------------------------------------------------

void SVideo::updateImageOpacity()
{
    ::fwData::Image::csptr img = this->getInput< ::fwData::Image >(s_IMAGE_INPUT);
    if(img->getField( "TRANSPARENCY" ) )
    {
        ::fwData::Integer::sptr transparency = img->getField< ::fwData::Integer >( "TRANSPARENCY" );
        const double imageOpacity = (100 - (*transparency) ) / 100.0;
        m_actor->SetOpacity(imageOpacity);
    }
    if(img->getField( "VISIBILITY" ) )
    {
        ::fwData::Boolean::sptr visible = img->getField< ::fwData::Boolean >( "VISIBILITY" );
        m_actor->SetVisibility(*visible);
    }

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SVideo::updateImage()
{
    m_isTextureInit = false;
    this->updating();
}

//------------------------------------------------------------------------------

void SVideo::show(bool visible)
{
    m_actor->SetVisibility(visible);

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SVideo::updateTF()
{
    ::fwData::TransferFunction::csptr tf = this->getInput< ::fwData::TransferFunction>(s_TF_INPUT);
    SLM_ASSERT("input '" + s_TF_INPUT + "' is missing.", tf);
    ::fwVtkIO::helper::TransferFunction::toVtkLookupTable(tf, m_lookupTable);
    this->requestRender();
}

//------------------------------------------------------------------------------

} //namespace visuVTKARAdaptor

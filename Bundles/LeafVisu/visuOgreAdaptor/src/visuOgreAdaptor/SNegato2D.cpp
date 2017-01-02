/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SNegato2D.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>

#include <OgreCamera.h>
#include <OgreSceneNode.h>
#include <OgreTextureManager.h>

#include <algorithm>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SNegato2D, ::fwData::Image);

const ::fwCom::Slots::SlotKeyType s_NEWIMAGE_SLOT   = "newImage";
const ::fwCom::Slots::SlotKeyType s_SLICETYPE_SLOT  = "sliceType";
const ::fwCom::Slots::SlotKeyType s_SLICEINDEX_SLOT = "sliceIndex";

//------------------------------------------------------------------------------

SNegato2D::SNegato2D() throw() :
    ::fwDataTools::helper::MedicalImageAdaptor(),
    m_plane(nullptr),
    m_negatoSceneNode(nullptr),
    m_filtering( ::fwRenderOgre::Plane::FilteringEnumType::NONE )
{
    SLM_TRACE_FUNC();

    this->installTFSlots(this);

    m_currentSliceIndex = {0.f, 0.f, 0.f};

    newSlot(s_NEWIMAGE_SLOT, &SNegato2D::newImage, this);
    newSlot(s_SLICETYPE_SLOT, &SNegato2D::changeSliceType, this);
    newSlot(s_SLICEINDEX_SLOT, &SNegato2D::changeSliceIndex, this);
}

//------------------------------------------------------------------------------

SNegato2D::~SNegato2D() throw()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SNegato2D::doStart() throw(::fwTools::Failed)
{
    ::fwData::Composite::sptr tfSelection = this->getInOut< ::fwData::Composite>("TF");
    this->setTransferFunctionSelection(tfSelection);
    this->setTFSelectionFwID(tfSelection->getID());

    this->updateImageInfos(this->getObject< ::fwData::Image >());
    this->updateTransferFunction(this->getImage());

    // 3D source texture instantiation
    m_3DOgreTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_Texture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    // TF texture initialization
    m_gpuTF = std::unique_ptr< ::fwRenderOgre::TransferFunction>(new ::fwRenderOgre::TransferFunction());
    m_gpuTF->createTexture(this->getID());

    // Scene node's instanciation
    m_negatoSceneNode = this->getSceneManager()->getRootSceneNode()->createChildSceneNode();

    // Plane's instanciation
    m_plane = new ::fwRenderOgre::Plane(this->getID(), m_negatoSceneNode, getSceneManager(),
                                        OrientationMode::X_AXIS, false, m_3DOgreTexture, m_filtering);

    this->getLayer()->getDefaultCamera()->setProjectionType( ::Ogre::ProjectionType::PT_ORTHOGRAPHIC );

    this->installTFConnections();

    bool isValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(this->getImage());
    if(isValid)
    {
        this->newImage();
    }
}

//------------------------------------------------------------------------------

void SNegato2D::doStop() throw(::fwTools::Failed)
{
    this->removeTFConnections();

    if (!m_connection.expired())
    {
        m_connection.disconnect();
    }

    m_plane->removeAndDestroyPlane();
    delete m_plane;

    m_3DOgreTexture.setNull();
    m_gpuTF.reset();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::doConfigure() throw(::fwTools::Failed)
{
    if(m_configuration->hasAttribute("sliceIndex"))
    {
        std::string orientation = m_configuration->getAttributeValue("sliceIndex");

        if(orientation == "axial")
        {
            m_orientation = Z_AXIS;
        }
        else if(orientation == "frontal")
        {
            m_orientation = Y_AXIS;
        }
        else if(orientation == "sagittal")
        {
            m_orientation = X_AXIS;
        }
    }
    else
    {
        // Axis orientation mode by default
        m_orientation = OrientationMode::Z_AXIS;
    }

    if(m_configuration->hasAttribute("filtering"))
    {
        std::string filteringValue = m_configuration->getAttributeValue("filtering");
        ::fwRenderOgre::Plane::FilteringEnumType filtering(::fwRenderOgre::Plane::FilteringEnumType::LINEAR);

        if(filteringValue == "none")
        {
            filtering = ::fwRenderOgre::Plane::FilteringEnumType::NONE;
        }
        else if(filteringValue == "anisotropic")
        {
            filtering = ::fwRenderOgre::Plane::FilteringEnumType::ANISOTROPIC;
        }

        this->setFiltering(filtering);
    }

    this->parseTFConfig(m_configuration);
}

//------------------------------------------------------------------------------

void SNegato2D::doUpdate() throw(::fwTools::Failed)
{
    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::newImage()
{
    if(m_3DOgreTexture.isNull())
    {
        // The adaptor hasn't start yet (the window is maybe not visible)
        return;
    }
    this->getRenderService()->makeCurrent();

    ::fwData::Image::sptr image = this->getImage();

    // Retrieves or creates the slice index fields
    this->updateImageInfos(image);

    ::fwRenderOgre::Utils::convertImageForNegato(m_3DOgreTexture.get(), image);

    this->createPlane( image->getSpacing() );
    this->updateCameraWindowBounds();

    // Update Slice
    this->changeSliceIndex(m_axialIndex->value(), m_frontalIndex->value(), m_sagittalIndex->value());

    // Update tranfer function in Gpu programs
    this->updatingTFPoints();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::changeSliceType(int _from, int _to)
{
    OrientationMode newOrientationMode;

    switch (_to)
    {
        case 0:
            newOrientationMode = OrientationMode::X_AXIS;
            break;
        case 1:
            newOrientationMode = OrientationMode::Y_AXIS;
            break;
        case 2:
            newOrientationMode = OrientationMode::Z_AXIS;
            break;
    }

    // The orientation update setter will change the fragment shader
    m_plane->setOrientationMode(newOrientationMode);

    // Update TF
    this->updatingTFWindowing(this->getTransferFunction()->getWindow(), this->getTransferFunction()->getLevel());

    // Update threshold if necessary
    this->updatingTFPoints();

    this->updateShaderSliceIndexParameter();
}

//------------------------------------------------------------------------------

void SNegato2D::changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex)
{
    ::fwData::Image::sptr image = this->getImage();

    m_axialIndex->value()    = _axialIndex;
    m_frontalIndex->value()  = _frontalIndex;
    m_sagittalIndex->value() = _sagittalIndex;

    m_currentSliceIndex = {
        static_cast<float>(_sagittalIndex ) / (static_cast<float>(image->getSize()[0] - 1)),
        static_cast<float>(_frontalIndex  ) / (static_cast<float>(image->getSize()[1] - 1)),
        static_cast<float>(_axialIndex    ) / (static_cast<float>(image->getSize()[2] - 1))
    };

    this->updateShaderSliceIndexParameter();
}

//------------------------------------------------------------------------------

void SNegato2D::updateShaderSliceIndexParameter()
{
    this->getRenderService()->makeCurrent();
    m_plane->changeSlice( m_currentSliceIndex[static_cast<size_t>(m_plane->getOrientationMode())] );

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::updatingTFPoints()
{
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    m_gpuTF->updateTexture(tf);

    m_plane->switchThresholding(tf->getIsClamped());

    // Sends the TF texture to the negato-related passes
    m_plane->setTFData(m_gpuTF->getTexture());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SNegato2D::updatingTFWindowing(double window, double level)
{
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    m_gpuTF->updateTexture(tf);

    this->requestRender();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SNegato2D::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( "image", ::fwData::Image::s_MODIFIED_SIG, s_NEWIMAGE_SLOT );
    connections.push( "image", ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_NEWIMAGE_SLOT );
    connections.push( "image", ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICETYPE_SLOT );
    connections.push( "image", ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICEINDEX_SLOT );

    return connections;

}

//------------------------------------------------------------------------------

void SNegato2D::createPlane(const fwData::Image::SpacingType& _spacing)
{
    this->getRenderService()->makeCurrent();
    // Fits the plane to the new texture
    m_plane->setDepthSpacing(_spacing);
    m_plane->initialize2DPlane();
}

//------------------------------------------------------------------------------

void SNegato2D::doSwap() throw(fwTools::Failed)
{
    this->stopping();
    this->starting();
    this->updating();
}

//------------------------------------------------------------------------------

void SNegato2D::updateCameraWindowBounds()
{

    ::Ogre::Real renderWindowWidth,renderWindowHeight, renderWindowRatio;
    ::Ogre::RenderSystem* renderSystem = getSceneManager()->getDestinationRenderSystem();

    renderWindowWidth  = static_cast< ::Ogre::Real >(renderSystem->_getViewport()->getActualWidth());
    renderWindowHeight = static_cast< ::Ogre::Real >(renderSystem->_getViewport()->getActualHeight());
    renderWindowRatio  = renderWindowWidth / renderWindowHeight;

    ::Ogre::Camera* cam = this->getLayer()->getDefaultCamera();

    if( renderWindowWidth < renderWindowHeight)
    {
        cam->setOrthoWindowWidth(m_plane->getWidth());
    }
    else
    {
        cam->setOrthoWindowHeight(m_plane->getHeight());
    }
    cam->setAspectRatio( renderWindowRatio );

    this->requestRender();
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor

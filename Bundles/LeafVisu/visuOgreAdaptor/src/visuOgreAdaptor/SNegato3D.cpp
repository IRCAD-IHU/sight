/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SNegato3D.hpp"

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwComEd/Dictionary.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>

#include <algorithm>

#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SNegato3D, ::fwData::Image);

const ::fwCom::Slots::SlotKeyType SNegato3D::s_NEWIMAGE_SLOT          = "newImage";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_SLICETYPE_SLOT         = "sliceType";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_SLICEINDEX_SLOT        = "sliceIndex";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_UPDATE_OPACITY_SLOT    = "updateOpacity";
const ::fwCom::Slots::SlotKeyType SNegato3D::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";

//------------------------------------------------------------------------------

SNegato3D::SNegato3D() throw() :
    m_autoResetCamera(true),
    m_activePlane(nullptr),
    m_negatoSceneNode(nullptr),
    m_filtering( ::fwRenderOgre::Plane::FilteringEnumType::NONE )
{
    this->installTFSlots(this);

    newSlot(s_NEWIMAGE_SLOT, &SNegato3D::newImage, this);
    newSlot(s_SLICETYPE_SLOT, &SNegato3D::changeSliceType, this);
    newSlot(s_SLICEINDEX_SLOT, &SNegato3D::changeSliceIndex, this);
    newSlot(s_UPDATE_OPACITY_SLOT, &SNegato3D::setPlanesOpacity, this);
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SNegato3D::setPlanesOpacity, this);
}

//------------------------------------------------------------------------------

SNegato3D::~SNegato3D() throw()
{
}

//------------------------------------------------------------------------------

void SNegato3D::doStart() throw(::fwTools::Failed)
{
    ::fwData::Composite::sptr tfSelection = this->getInOut< ::fwData::Composite>("TF");
    SLM_ASSERT("TF 'key' not found", tfSelection);

    this->setTransferFunctionSelection(tfSelection);
    this->setTFSelectionFwID(tfSelection->getID());

    this->updateImageInfos(this->getObject< ::fwData::Image >());
    this->updateTransferFunction(this->getImage());

    // 3D source texture instantiation
    m_3DOgreTexture = ::Ogre::TextureManager::getSingleton().createOrRetrieve(
        this->getID() + "_Texture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true).first.dynamicCast< ::Ogre::Texture>();

    // TF texture initialization
    m_gpuTF = std::unique_ptr< ::fwRenderOgre::TransferFunction>(new ::fwRenderOgre::TransferFunction());
    m_gpuTF->createTexture(this->getID());

    // Scene node's instantiation
    m_negatoSceneNode = this->getSceneManager()->getRootSceneNode()->createChildSceneNode();

    // Instanciation of the planes
    m_planes[0] = new ::fwRenderOgre::Plane(this->getID(), m_negatoSceneNode,
                                            getSceneManager(),
                                            OrientationMode::X_AXIS, true, m_3DOgreTexture, m_filtering);
    m_planes[1] = new ::fwRenderOgre::Plane(this->getID(), m_negatoSceneNode,
                                            getSceneManager(),
                                            OrientationMode::Y_AXIS, true, m_3DOgreTexture, m_filtering);
    m_planes[2] = new ::fwRenderOgre::Plane(this->getID(), m_negatoSceneNode,
                                            getSceneManager(),
                                            OrientationMode::Z_AXIS, true, m_3DOgreTexture, m_filtering);

    this->setPlanesOpacity();

    // Default active plane = sagittal plane.
    m_activePlane = m_planes[0];

    if (m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    this->installTFConnections();

    bool isValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(this->getImage());
    if (isValid)
    {
        this->newImage();
    }
}

//------------------------------------------------------------------------------

void SNegato3D::doStop() throw(::fwTools::Failed)
{
    this->removeTFConnections();

    m_planes[0]->removeAndDestroyPlane();
    m_planes[1]->removeAndDestroyPlane();
    m_planes[2]->removeAndDestroyPlane();

    delete m_planes[0];
    delete m_planes[1];
    delete m_planes[2];

    m_3DOgreTexture.setNull();
    m_gpuTF.reset();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::doConfigure() throw(::fwTools::Failed)
{
    // Axis orientation mode by default
    m_orientation = OrientationMode::Z_AXIS;

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
    if(m_configuration->hasAttribute("autoresetcamera"))
    {
        std::string autoResetCamera = m_configuration->getAttributeValue("autoresetcamera");
        m_autoResetCamera = (autoResetCamera == "yes");
    }
    if(m_configuration->hasAttribute("transform"))
    {
        this->setTransformId(m_configuration->getAttributeValue("transform"));
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

void SNegato3D::doUpdate() throw(::fwTools::Failed)
{
    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::doSwap() throw(::fwTools::Failed)
{
    this->stopping();
    this->starting();
    this->updating();
}

//------------------------------------------------------------------------------

void SNegato3D::createPlanes(const ::fwData::Image::SpacingType& _spacing, const ::fwData::Image::OriginType& _origin)
{
    ::Ogre::Vector3 origin(static_cast< ::Ogre::Real >(_origin[0]),
                           static_cast< ::Ogre::Real >(_origin[1]),
                           static_cast< ::Ogre::Real >(_origin[2]));

    // Fits the planes to the new texture
    for(int i(0); i < 3; ++i)
    {
        m_planes[i]->setDepthSpacing(_spacing);
        m_planes[i]->setOriginPosition(origin);
        m_planes[i]->initialize3DPlane();
    }
}

//------------------------------------------------------------------------------

void SNegato3D::newImage()
{
    this->getRenderService()->makeCurrent();

    ::fwData::Image::sptr image = this->getImage();

    // Retrieves or creates the slice index fields
    this->updateImageInfos(image);

    ::fwRenderOgre::Utils::convertImageForNegato(m_3DOgreTexture.get(), image);

    createPlanes(image->getSpacing(), image->getOrigin());

    // Update Slice
    this->changeSliceIndex(m_axialIndex->value(), m_frontalIndex->value(), m_sagittalIndex->value());

    // Update tranfer function in Gpu programs
    this->updatingTFPoints();

    if (m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }
    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::changeSliceType(int _from, int _to)
{
    // We have to update the active plane
    m_activePlane = m_planes[_to];

    this->getRenderService()->makeCurrent();

    // Update TF
    this->updatingTFWindowing(this->getTransferFunction()->getWindow(), this->getTransferFunction()->getLevel());

    // Update threshold if necessary
    this->updatingTFPoints();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex)
{
    ::fwData::Image::sptr image = this->getImage();

    float sliceIndex[3] = {
        static_cast<float>(_sagittalIndex ) / (static_cast<float>(image->getSize()[0] - 1)),
        static_cast<float>(_frontalIndex  ) / (static_cast<float>(image->getSize()[1] - 1)),
        static_cast<float>(_axialIndex    ) / (static_cast<float>(image->getSize()[2] - 1))
    };

    for (int i = 0; i < 3; ++i)
    {
        m_planes[i]->changeSlice( sliceIndex[i] );
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SNegato3D::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( "image", ::fwData::Image::s_MODIFIED_SIG, s_NEWIMAGE_SLOT );
    connections.push( "image", ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_NEWIMAGE_SLOT );
    connections.push( "image", ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICETYPE_SLOT );
    connections.push( "image", ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICEINDEX_SLOT );
    connections.push( "image", ::fwData::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_VISIBILITY_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

void SNegato3D::updatingTFPoints()
{
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    m_gpuTF->updateTexture(tf);

    for(int i(0); i < 3; ++i)
    {
        m_planes[i]->switchThresholding(tf->getIsClamped());

        // Sends the TF texture to the negato-related passes
        m_planes[i]->setTFData(m_gpuTF->getTexture());
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SNegato3D::updatingTFWindowing(double window, double level)
{
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    m_gpuTF->updateTexture(tf);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SNegato3D::setPlanesOpacity()
{
    ::fwData::Image::sptr image = this->getImage();

    const std::string TRANSPARENCY_FIELD = "TRANSPARENCY";
    const std::string VISIBILITY_FIELD   = "VISIBILITY";

    ::fwData::Integer::sptr transparency = image->setDefaultField(TRANSPARENCY_FIELD, ::fwData::Integer::New(0));
    ::fwData::Boolean::sptr isVisible    = image->setDefaultField(VISIBILITY_FIELD, ::fwData::Boolean::New(true));

    float opacity = isVisible->getValue() ? (100.f - transparency->getValue())/100.f : 0.f;

    if(m_planes[0] && m_planes[1] && m_planes[2])
    {
        m_planes[0]->setEntityOpacity(opacity);
        m_planes[1]->setEntityOpacity(opacity);
        m_planes[2]->setEntityOpacity(opacity);
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor

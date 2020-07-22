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

#include "visuOgreAdaptor/SNegato3D.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/Color.hpp>
#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwRenderOgre/compositor/Core.hpp>
#include <fwRenderOgre/picker/IPicker.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreVector.h>

#include <algorithm>

namespace visuOgreAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_NEWIMAGE_SLOT       = "newImage";
static const ::fwCom::Slots::SlotKeyType s_SLICETYPE_SLOT      = "sliceType";
static const ::fwCom::Slots::SlotKeyType s_SLICEINDEX_SLOT     = "sliceIndex";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_OPACITY_SLOT = "updateOpacity";

static const ::fwCom::Signals::SignalKeyType s_PICKED_VOXEL_SIG = "pickedVoxel";

static const std::string s_IMAGE_INOUT = "image";
static const std::string s_TF_INOUT    = "tf";

static const std::string s_AUTORESET_CAMERA_CONFIG = "autoresetcamera";
static const std::string s_TRANSFORM_CONFIG        = "transform";
static const std::string s_FILTERING_CONFIG        = "filtering";
static const std::string s_TF_ALPHA_CONFIG         = "tfAlpha";
static const std::string s_INTERACTIVE_CONFIG      = "interactive";
static const std::string s_PRIORITY_CONFIG         = "priority";
static const std::string s_QUERY_CONFIG            = "queryFlags";
static const std::string s_BORDER_CONFIG           = "border";

static const std::string s_TRANSPARENCY_FIELD = "TRANSPARENCY";
static const std::string s_VISIBILITY_FIELD   = "VISIBILITY";

static constexpr std::uint8_t s_NEGATO_WIDGET_RQ_GROUP_ID = ::fwRenderOgre::compositor::Core::s_SURFACE_RQ_GROUP_ID - 1;

//------------------------------------------------------------------------------

SNegato3D::SNegato3D() noexcept :
    m_helperTF(std::bind(&SNegato3D::updateTF, this))
{
    newSlot(s_NEWIMAGE_SLOT, &SNegato3D::newImage, this);
    newSlot(s_SLICETYPE_SLOT, &SNegato3D::changeSliceType, this);
    newSlot(s_SLICEINDEX_SLOT, &SNegato3D::changeSliceIndex, this);
    newSlot(s_UPDATE_OPACITY_SLOT, &SNegato3D::setPlanesOpacity, this);
    newSlot("setVisibility", &SNegato3D::updateVisibility, this);

    m_pickedVoxelSignal = newSignal<PickedVoxelSigType>(s_PICKED_VOXEL_SIG);
}

//------------------------------------------------------------------------------

SNegato3D::~SNegato3D() noexcept
{
}

//------------------------------------------------------------------------------

void SNegato3D::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_autoResetCamera = config.get<std::string>(s_AUTORESET_CAMERA_CONFIG, "yes") == "yes";

    if(config.count(s_FILTERING_CONFIG))
    {
        const std::string filteringValue = config.get<std::string>(s_FILTERING_CONFIG);
        ::fwRenderOgre::Plane::FilteringEnumType filtering(::fwRenderOgre::Plane::FilteringEnumType::LINEAR);

        if(filteringValue == "none")
        {
            filtering = ::fwRenderOgre::Plane::FilteringEnumType::NONE;
        }
        else if(filteringValue == "anisotropic")
        {
            filtering = ::fwRenderOgre::Plane::FilteringEnumType::ANISOTROPIC;
        }

        m_filtering = filtering;
    }

    const std::string hexaMask = config.get<std::string>(s_QUERY_CONFIG, "");
    if(!hexaMask.empty())
    {
        SLM_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask,
            hexaMask.length() > 2 &&
            hexaMask.substr(0, 2) == "0x");
        m_queryFlags = static_cast< std::uint32_t >(std::stoul(hexaMask, nullptr, 16));
    }

    m_enableAlpha = config.get<bool>(s_TF_ALPHA_CONFIG, m_enableAlpha);
    m_interactive = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);
    m_priority    = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_border      = config.get<bool>(s_BORDER_CONFIG, m_border);

    const std::string transformId =
        config.get<std::string>(::fwRenderOgre::ITransformable::s_TRANSFORM_CONFIG, this->getID() + "_transform");
    this->setTransformId(transformId);
}

//------------------------------------------------------------------------------

void SNegato3D::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' does not exist", image);

    ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
    m_helperTF.setOrCreateTF(tf, image);

    // TF texture initialization
    m_gpuTF = std::make_unique< ::fwRenderOgre::TransferFunction>();
    m_gpuTF->createTexture(this->getID());

    // 3D source texture instantiation
    m_3DOgreTexture = ::Ogre::TextureManager::getSingleton().create(
        this->getID() + "_Negato3DTexture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    // Scene node's instantiation
    ::Ogre::SceneNode* const rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* const transformNode = this->getTransformNode(rootSceneNode);
    m_negatoSceneNode                      = transformNode->createChildSceneNode();

    // Instanciation of the planes
    int orientationNum { 0 };
    for(auto& plane : m_planes)
    {
        auto imgOrientation = static_cast<OrientationMode>(orientationNum++);
        plane = std::make_shared< ::fwRenderOgre::Plane >(this->getID(), m_negatoSceneNode,
                                                          this->getSceneManager(),
                                                          imgOrientation, m_3DOgreTexture,
                                                          m_filtering, m_border);
    }

    if (m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    this->newImage();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast< ::fwRenderOgre::interactor::IInteractor >(this->getSptr());
        this->getLayer()->addInteractor(interactor, m_priority);

        m_pickingCross = this->getSceneManager()->createManualObject(this->getID() + "_PickingCross");
        const auto basicAmbientMat = ::Ogre::MaterialManager::getSingleton().getByName("BasicAmbient");
        auto crossMat              = basicAmbientMat->clone(this->getID() + "_CrossMaterial");
        crossMat->setAmbient(::Ogre::ColourValue::Red);
        crossMat->setDiffuse(::Ogre::ColourValue::Red);
        crossMat->setDepthCheckEnabled(false);
        m_pickingCross->estimateVertexCount(4);
        m_pickingCross->begin(crossMat, ::Ogre::RenderOperation::OT_LINE_LIST);
        for(std::uint8_t i = 0; i < 4; ++i)
        {
            m_pickingCross->position(::Ogre::Vector3::ZERO);
        }
        m_pickingCross->end();
        m_pickingCross->setVisible(false);
        // Always render the widget after the plane it is displayed on.
        m_pickingCross->setRenderQueueGroupAndPriority(s_NEGATO_WIDGET_RQ_GROUP_ID, 1);
        m_negatoSceneNode->attachObject(m_pickingCross);
    }

    // Set the visibility of the 3D Negato
    this->setVisible(m_isVisible);
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SNegato3D::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_NEWIMAGE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_NEWIMAGE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICETYPE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICEINDEX_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_VISIBILITY_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_TRANSPARENCY_MODIFIED_SIG, s_UPDATE_OPACITY_SLOT );
    return connections;
}

//------------------------------------------------------------------------------

void SNegato3D::updating()
{
    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::swapping(const KeyType& key)
{
    if (key == s_TF_INOUT)
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' does not exist", image);

        ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
        m_helperTF.setOrCreateTF(tf, image);

        this->updateTF();
    }
}

//------------------------------------------------------------------------------

void SNegato3D::stopping()
{
    this->getRenderService()->makeCurrent();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast< ::fwRenderOgre::interactor::IInteractor >(this->getSptr());
        this->getLayer()->removeInteractor(interactor);
    }

    m_helperTF.removeTFConnections();

    m_pickedPlane.reset();

    for(auto& plane : m_planes)
    {
        plane.reset();
    }

    m_negatoSceneNode->removeAndDestroyAllChildren();
    this->getSceneManager()->destroySceneNode(m_negatoSceneNode);

    if(m_pickingCross)
    {
        auto crossMtl = m_pickingCross->getSection(0)->getMaterial();
        ::Ogre::MaterialManager::getSingleton().remove(crossMtl);

        this->getSceneManager()->destroyManualObject(m_pickingCross);
    }

    ::Ogre::TextureManager::getSingleton().remove(m_3DOgreTexture);

    m_3DOgreTexture.reset();
    m_gpuTF.reset();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::createPlanes(const ::Ogre::Vector3& _spacing, const ::Ogre::Vector3& _origin)
{
    // Fits the planes to the new texture
    for(const auto& plane : m_planes)
    {
        plane->setVoxelSpacing(_spacing);
        plane->setOriginPosition(_origin);
        plane->initializePlane();
        plane->enableAlpha(m_enableAlpha);
        plane->setQueryFlags(m_queryFlags);
    }
}

//------------------------------------------------------------------------------

void SNegato3D::newImage()
{
    this->getRenderService()->makeCurrent();
    {
        const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' does not exist", image);

        ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
        m_helperTF.setOrCreateTF(tf, image);

        const ::fwData::mt::ObjectReadLock imageLock(image);

        if(!::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
        {
            return;
        }

        ::fwRenderOgre::Utils::convertImageForNegato(m_3DOgreTexture.get(), image);

        const auto [spacing, origin] = ::fwRenderOgre::Utils::convertSpacingAndOrigin(image);
        this->createPlanes(spacing, origin);

        // Update Slice
        const auto axialIndex =
            image->getField< ::fwData::Integer >(::fwDataTools::fieldHelper::Image::m_axialSliceIndexId)->getValue();
        const auto frontalIndex =
            image->getField< ::fwData::Integer >(::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId)->getValue();
        const auto sagittalIndex =
            image->getField< ::fwData::Integer >(::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId)->getValue();

        this->changeSliceIndex(
            static_cast<int>(axialIndex),
            static_cast<int>(frontalIndex),
            static_cast<int>(sagittalIndex)
            );
    }

    // Update tranfer function in Gpu programs
    this->updateTF();

    if (m_autoResetCamera)
    {
        this->getRenderService()->resetCameraCoordinates(m_layerID);
    }

    this->setPlanesOpacity();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::changeSliceType(int, int)
{
    this->getRenderService()->makeCurrent();

    this->updateTF();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato3D::changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex)
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' does not exist", image);
    const ::fwData::mt::ObjectReadLock imgLock(image);

    const auto& imgSize = image->getSize2();

    const ::Ogre::Vector3 sliceIndices = {
        static_cast<float>(_sagittalIndex ) / (static_cast<float>(imgSize[0] - 1)),
        static_cast<float>(_frontalIndex  ) / (static_cast<float>(imgSize[1] - 1)),
        static_cast<float>(_axialIndex    ) / (static_cast<float>(imgSize[2] - 1))
    };

    for (std::uint8_t i = 0; i < 3; ++i)
    {
        m_planes[i]->changeSlice( sliceIndices[i] );
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SNegato3D::updateTF()
{
    const ::fwData::TransferFunction::csptr tf = m_helperTF.getTransferFunction();
    {
        const ::fwData::mt::ObjectReadLock tfLock(tf);
        m_gpuTF->updateTexture(tf);

        for(const auto& plane : m_planes)
        {
            plane->switchThresholding(tf->getIsClamped());

            // Sends the TF texture to the negato-related passes
            plane->setTFData(*m_gpuTF.get());
        }
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SNegato3D::setPlanesOpacity()
{
    const auto notNull = std::bind(std::not_equal_to< ::fwRenderOgre::Plane::sptr >(), std::placeholders::_1, nullptr);

    if(std::all_of(m_planes.begin(), m_planes.end(), notNull))
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' does not exist", image);
        const ::fwData::mt::ObjectReadLock imageLock(image);

        const auto transparency = image->setDefaultField(s_TRANSPARENCY_FIELD, ::fwData::Integer::New(0));
        const auto isVisible    = image->setDefaultField(s_VISIBILITY_FIELD, ::fwData::Boolean::New(true));

        const bool visible  = isVisible->getValue();
        const float opacity = (100.f - static_cast<float>(transparency->getValue()))/100.f;

        for(const auto& plane : m_planes)
        {
            plane->setVisible(visible);
            plane->setEntityOpacity(opacity);
        }

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SNegato3D::setVisible(bool _visible)
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' does not exist", image);
    const ::fwData::mt::ObjectReadLock imageLock(image);

    image->setField(s_VISIBILITY_FIELD, ::fwData::Boolean::New(_visible));

    this->setPlanesOpacity();

    using VisModSigType = ::fwData::Image::VisibilityModifiedSignalType;
    auto visUpdateSig = image->signal<VisModSigType>( ::fwData::Image::s_VISIBILITY_MODIFIED_SIG );

    {
        const ::fwCom::Connection::Blocker blocker(visUpdateSig->getConnection(this->slot(s_UPDATE_VISIBILITY_SLOT)));
        visUpdateSig->asyncEmit(_visible);
    }
}

//------------------------------------------------------------------------------

void SNegato3D::setVisibilityDeprecatedSlot(bool _visible)
{
    FW_DEPRECATED_MSG("::visuOgreAdaptor::SNegato3D::setVisibility is no longer supported", "21.0")
    ::fwRenderOgre::IAdaptor::updateVisibility(_visible);
}

//------------------------------------------------------------------------------

void SNegato3D::setPlanesQueryFlags(std::uint32_t _flags)
{
    for(const auto& p : m_planes)
    {
        p->setQueryFlags(_flags);
    }
}

//------------------------------------------------------------------------------

void SNegato3D::mouseMoveEvent(MouseButton _button, Modifier, int _x, int _y, int, int)
{
    if(m_pickedPlane)
    {
        if(_button == MouseButton::MIDDLE)
        {
            this->moveSlices(_x, _y);
        }
        else if(_button == MouseButton::RIGHT)
        {
            const double dx = static_cast<double>(_x - m_initialPos[0]);
            const double dy = static_cast<double>(m_initialPos[1] - _y);

            this->updateWindowing(dx, dy);
        }
        else if(_button == MouseButton::LEFT)
        {
            this->pickIntensity(_x, _y);
        }

        this->getLayer()->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

void SNegato3D::buttonPressEvent(MouseButton _button, Modifier, int _x, int _y)
{
    m_pickedPlane.reset();
    m_pickingCross->setVisible(false);

    if(_button == MouseButton::MIDDLE)
    {
        this->moveSlices(_x, _y);
    }
    else if(_button == MouseButton::RIGHT)
    {
        if(this->getPickedSlices(_x, _y) != std::nullopt)
        {
            const ::fwData::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
            const ::fwData::mt::ObjectReadLock tfLock(tf);

            m_initialLevel  = tf->getLevel();
            m_initialWindow = tf->getWindow();

            m_initialPos = { _x, _y };
        }
    }
    else if(_button == MouseButton::LEFT)
    {
        this->pickIntensity(_x, _y);
    }

    if(m_pickedPlane)
    {
        this->getLayer()->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

void SNegato3D::buttonReleaseEvent(MouseButton, Modifier, int, int)
{
    if(m_pickedPlane)
    {
        m_pickedPlane->setRenderQueuerGroupAndPriority(::fwRenderOgre::compositor::Core::s_SURFACE_RQ_GROUP_ID, 0);
        m_pickedPlane.reset();
    }
    m_pickingCross->setVisible(false);
    m_pickedVoxelSignal->asyncEmit("");
    this->setPlanesQueryFlags(m_queryFlags); // Make all planes pickable again.
}

//------------------------------------------------------------------------------

void SNegato3D::moveSlices(int _x, int _y)
{
    const auto pickRes = this->getPickedSlices(_x, _y);

    if(pickRes.has_value())
    {
        const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' does not exist", image);
        const ::fwData::mt::ObjectReadLock imgLock(image);

        auto pickedPt = pickRes.value();

        for(const auto&  p : m_planes)
        {
            if(p != m_pickedPlane)
            {
                p->setQueryFlags(0x0);
            }
        }

        const auto [spacing, origin] = ::fwRenderOgre::Utils::convertSpacingAndOrigin(image);
        pickedPt                     = (pickedPt - origin) / spacing;

        const ::Ogre::Vector3i pickedPtI(pickedPt);
        const auto sig = image->signal< ::fwData::Image::SliceIndexModifiedSignalType >
                             (::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG);
        sig->asyncEmit(pickedPtI[2], pickedPtI[1], pickedPtI[0]);
    }
}

//------------------------------------------------------------------------------

void SNegato3D::pickIntensity(int _x, int _y)
{
    if(m_pickedVoxelSignal->getNumberOfConnections() > 0)
    {
        const auto pickedPos = this->getPickedSlices(_x, _y);

        if(pickedPos.has_value())
        {
            const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
            SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' does not exist", image);
            const ::fwData::mt::ObjectReadLock imgLock(image);

            if(!::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
            {
                return;
            }

            const auto imageBufferLock = image->lock();

            const auto [spacing, origin] = ::fwRenderOgre::Utils::convertSpacingAndOrigin(image);
            const auto pickedPosImageSpace = (pickedPos.value() - origin) / spacing;

            this->updatePickingCross(pickedPos.value(), origin);

            const auto& imgSize = image->getSize2();
            ::fwData::Image::Size pickedVoxel;
            for(std::uint8_t i = 0; i < pickedVoxel.size(); ++i)
            {
                pickedVoxel[i] = std::clamp(static_cast<size_t>(pickedPosImageSpace[i]), size_t{0}, imgSize[i] - 1);
            }

            const auto intensity   = image->getPixelAsString(pickedVoxel[0], pickedVoxel[1], pickedVoxel[2]);
            const auto pickingText = "(" + std::to_string(pickedVoxel[0]) + ", " + std::to_string(pickedVoxel[1])
                                     + ", " + std::to_string(pickedVoxel[2]) + "): " + intensity;

            m_pickedVoxelSignal->asyncEmit(pickingText);

            // Render the picked plane before the widget.
            m_pickedPlane->setRenderQueuerGroupAndPriority(s_NEGATO_WIDGET_RQ_GROUP_ID, 0);
        }
    }
}

//------------------------------------------------------------------------------

std::optional< ::Ogre::Vector3 > SNegato3D::getPickedSlices(int _x, int _y)
{
    auto* const sceneManager = this->getSceneManager();
    SLM_ASSERT("Scene manager not created yet.", sceneManager);

    ::fwRenderOgre::picker::IPicker picker;
    picker.setSceneManager(sceneManager);
    picker.executeRaySceneQuery(_x, _y, m_queryFlags);

    const auto isPicked = [&picker](const ::fwRenderOgre::Plane::sptr& _p)
                          {
                              return (_p->getMovableObject() != nullptr) &&
                                     (_p->getMovableObject() == picker.getSelectedObject());
                          };

    auto it = std::find_if(m_planes.cbegin(), m_planes.cend(), isPicked);

    if(it != m_planes.cend())
    {
        m_pickedPlane = *it;
        return picker.getIntersectionInWorldSpace();
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void SNegato3D::updatePickingCross(const ::Ogre::Vector3& _pickedPos, const ::Ogre::Vector3& _imgOrigin)
{
    const float h = m_pickedPlane->getHeight();
    const float w = m_pickedPlane->getWidth();

    m_pickingCross->beginUpdate(0);
    switch(m_pickedPlane->getOrientationMode())
    {
        case ::fwRenderOgre::Plane::OrientationMode::X_AXIS:
            m_pickingCross->position(_pickedPos.x, 0 + _imgOrigin.y, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, h + _imgOrigin.y, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, _pickedPos.y, 0 + _imgOrigin.z);
            m_pickingCross->position(_pickedPos.x, _pickedPos.y, w + _imgOrigin.z);
            break;
        case ::fwRenderOgre::Plane::OrientationMode::Y_AXIS:
            m_pickingCross->position(0 + _imgOrigin.x, _pickedPos.y, _pickedPos.z);
            m_pickingCross->position(w + _imgOrigin.x, _pickedPos.y, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, _pickedPos.y, 0 + _imgOrigin.z);
            m_pickingCross->position(_pickedPos.x, _pickedPos.y, h + _imgOrigin.z);
            break;
        case ::fwRenderOgre::Plane::OrientationMode::Z_AXIS:
            m_pickingCross->position(0 + _imgOrigin.x, _pickedPos.y, _pickedPos.z);
            m_pickingCross->position(w + _imgOrigin.x, _pickedPos.y, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, 0 + _imgOrigin.y, _pickedPos.z);
            m_pickingCross->position(_pickedPos.x, h + _imgOrigin.y, _pickedPos.z);
            break;
    }
    m_pickingCross->end();
    m_pickingCross->setVisible(true);
}

//------------------------------------------------------------------------------

void SNegato3D::updateWindowing( double _dw, double _dl )
{
    const double newWindow = m_initialWindow + _dw;
    const double newLevel  = m_initialLevel - _dl;

    const ::fwData::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
    {
        const ::fwData::mt::ObjectWriteLock tfLock(tf);

        tf->setWindow( newWindow );
        tf->setLevel( newLevel );
        const auto sig = tf->signal< ::fwData::TransferFunction::WindowingModifiedSignalType >(
            ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG);
        {
            sig->asyncEmit(newWindow, newLevel);
        }
    }
}

} // namespace visuOgreAdaptor.

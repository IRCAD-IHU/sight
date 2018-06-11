/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderVTK/IAdaptor.hpp"

#include "fwRenderVTK/registry/adaptors.hpp"

#include <fwData/String.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTools/fwID.hpp>

#include <vtkAbstractPropPicker.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>

namespace fwRenderVTK
{

IAdaptor::IAdaptor() noexcept :
    m_vtkPipelineModified(true),
    m_rendererId("default"),
    m_pickerId(""),
    // by default no Picker
    m_transformId(""),
    // by default no Transform
    m_propCollection( vtkPropCollection::New() )
{
}

IAdaptor::~IAdaptor() noexcept
{
    m_propCollection->Delete();
}

//------------------------------------------------------------------------------

void IAdaptor::configureParams()
{
    const ConfigType config = this->getConfigTree();
    this->setPickerId(config.get<std::string>("config.<xmlattr>.picker", ""));
    this->setRendererId(config.get<std::string>("config.<xmlattr>.renderer", ""));
    this->setTransformId(config.get<std::string>("config.<xmlattr>.transform", ""));
}

//------------------------------------------------------------------------------

void IAdaptor::initialize()
{
    if(m_renderService.expired())
    {
        // retrieve the SRender service associated to this adaptor
        const auto servicesVector = ::fwServices::OSR::getServices("::fwRenderVTK::SRender");

        const auto& registry = ::fwRenderVTK::registry::getAdaptorRegistry();

        const std::string adaptorID = this->getID();
        SLM_ASSERT("The service '" + adaptorID + "' is not found in the VTK adaptor registry. Make sure the service is "
                   "declared in a SRender.", registry.find(adaptorID) != registry.end());
        const auto& renderServiceId = registry.at(adaptorID);

        auto result =
            std::find_if(servicesVector.begin(), servicesVector.end(),
                         [renderServiceId](const ::fwServices::IService::sptr& srv)
            {
                return srv->getID() == renderServiceId;
            });
        SLM_ASSERT("Can't find '" + renderServiceId + "' SRender service.", result != servicesVector.end());

        ::fwRenderVTK::SRender::sptr renderService = ::fwRenderVTK::SRender::dynamicCast(*result);
        m_renderService                            = renderService;
    }
}

//------------------------------------------------------------------------------

void IAdaptor::setRenderService( SRender::sptr service)
{
    /// Preconditions
    SLM_ASSERT("service not instanced", service);

    m_renderService = service;
}

//------------------------------------------------------------------------------

void IAdaptor::setRendererId(SRender::RendererIdType newID)
{
    m_rendererId = newID;
}

//------------------------------------------------------------------------------

void IAdaptor::setVtkPipelineModified()
{
    m_vtkPipelineModified = true;
}

//------------------------------------------------------------------------------

bool IAdaptor::getAutoRender() const
{
    return (m_renderService.lock()->getRenderMode() == SRender::RenderMode::AUTO);
}

//------------------------------------------------------------------------------

void IAdaptor::requestRender()
{
    if ( (this->getRenderService()->getStatus() == ::fwServices::IService::STARTED ||
          this->getRenderService()->getStatus() == ::fwServices::IService::SWAPPING)
         && this->getRenderService()->isShownOnScreen()
         && m_vtkPipelineModified && this->getAutoRender() )
    {
        if ( !this->getRenderService()->getPendingRenderRequest())
        {
            this->getRenderService()->setPendingRenderRequest(true);
            this->getRenderService()->slot(SRender::s_RENDER_SLOT)->asyncRun();
        }
        m_vtkPipelineModified = false;
    }
}

//------------------------------------------------------------------------------

SRender::RendererIdType IAdaptor::getRendererId() const
{
    return m_rendererId;
}

//------------------------------------------------------------------------------

SRender::sptr IAdaptor:: getRenderService() const
{
    SLM_ASSERT("SRender service is not initialized, initialize() method must be called first",
               !m_renderService.expired());
    return m_renderService.lock();
}

//------------------------------------------------------------------------------

vtkRenderer* IAdaptor::getRenderer()
{
    return m_renderService.lock()->getRenderer(m_rendererId);
}

//------------------------------------------------------------------------------

void IAdaptor::setPickerId(SRender::PickerIdType newID)
{
    m_pickerId = newID;
}

//------------------------------------------------------------------------------

SRender::PickerIdType IAdaptor::getPickerId() const
{
    return m_pickerId;
}

//------------------------------------------------------------------------------

vtkAbstractPropPicker* IAdaptor::getPicker(std::string pickerId)
{
    if (pickerId.empty())
    {
        pickerId = m_pickerId;
    }
    return m_renderService.lock()->getPicker(pickerId);
}

//------------------------------------------------------------------------------

void IAdaptor::setTransformId(SRender::VtkObjectIdType newID)
{
    m_transformId = newID;
}

//------------------------------------------------------------------------------

SRender::VtkObjectIdType IAdaptor::getTransformId() const
{
    return m_transformId;
}

//------------------------------------------------------------------------------

vtkTransform* IAdaptor::getTransform()
{
    SLM_ASSERT("Transform id must be defined", !m_transformId.empty());
    return m_renderService.lock()->getOrAddVtkTransform(m_transformId);
}

//------------------------------------------------------------------------------

vtkObject* IAdaptor::getVtkObject(const SRender::VtkObjectIdType& objectId) const
{
    if (!objectId.empty())
    {
        return m_renderService.lock()->getVtkObject(objectId);
    }
    return nullptr;
}

//------------------------------------------------------------------------------

vtkRenderWindowInteractor* IAdaptor::getInteractor()
{
    return this->getRenderer()->GetRenderWindow()->GetInteractor();
}

//------------------------------------------------------------------------------

IAdaptor::sptr IAdaptor::getAssociatedAdaptor(vtkProp* prop, int depth)
{
    IAdaptor::sptr srv;

    if (prop)
    {
        if ( m_propCollection->IsItemPresent(prop) )
        {
            srv = this->getSptr();
        }
        else
        {
            IAdaptor::sptr res;
            auto subServices = this->getRegisteredServices();
            for( auto& service : subServices)
            {
                auto adaptor = ::fwRenderVTK::IAdaptor::dynamicCast(service.lock());
                if(adaptor)
                {
                    res = adaptor->getAssociatedAdaptor(prop, depth - 1 );
                    if (res)
                    {
                        break;
                    }
                }
            }
            srv = ( res && depth == 0 ) ? this->getSptr() : res;
        }
    }
    return srv;
}

//------------------------------------------------------------------------------

void IAdaptor::registerProp(vtkProp* prop)
{
    getProps(m_propCollection, prop);
}

//------------------------------------------------------------------------------

void IAdaptor::getProps(vtkPropCollection* propc, vtkProp* prop)
{
    int initSize = propc->GetNumberOfItems();

    prop->GetActors(propc);
    prop->GetActors2D(propc);
    prop->GetVolumes(propc);

    if (initSize == propc->GetNumberOfItems())
    {
        propc->AddItem(prop);
    }
}

//------------------------------------------------------------------------------

void IAdaptor::getAllSubProps(vtkPropCollection* propc, int depth)
{
    vtkProp* prop;

    m_propCollection->InitTraversal();
    while ( (prop = m_propCollection->GetNextProp()) )
    {
        getProps(propc, prop);
    }

    if(depth != 0)
    {
        auto subServices = this->getRegisteredServices();
        for( const auto& service : subServices)
        {
            auto adaptor = ::fwRenderVTK::IAdaptor::dynamicCast(service.lock());
            if(adaptor)
            {
                adaptor->getAllSubProps( propc, depth - 1 );
            }
        }
    }
}

//------------------------------------------------------------------------------

void IAdaptor::unregisterProps()
{
    m_propCollection->RemoveAllItems();
}

//------------------------------------------------------------------------------

void IAdaptor::addToRenderer(vtkProp* prop)
{
    this->registerProp(prop);
    this->getRenderer()->AddViewProp(prop);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void IAdaptor::addToPicker(vtkProp* prop, std::string pickerId)
{
    OSLM_ASSERT("Picker '"<< pickerId << "' undefined.", this->getPicker(pickerId));
    this->getPicker(pickerId)->AddPickList(prop);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void IAdaptor::removeFromPicker(vtkProp* prop, std::string pickerId)
{
    OSLM_ASSERT("Picker '"<< pickerId << "' undefined.", this->getPicker(pickerId));
    this->getPicker(pickerId)->DeletePickList(prop);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void IAdaptor::removeAllPropFromRenderer()
{
    vtkPropCollection* propc = m_propCollection;
    vtkProp* prop;

    propc->InitTraversal();
    while ( (prop = propc->GetNextProp()) )
    {
        this->getRenderer()->RemoveViewProp(prop);
    }
    this->unregisterProps();
    this->setVtkPipelineModified();
}

} // namespace fwRenderVTK

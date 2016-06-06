/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/interactor/VRWidgetsInteractor.hpp"

#include <fwCom/Signal.hxx>

#include <fwRenderOgre/registry/macros.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreViewport.h>

fwRenderOgreRegisterInteractorMacro( ::fwRenderOgre::interactor::VRWidgetsInteractor );

namespace fwRenderOgre
{

namespace interactor
{

//------------------------------------------------------------------------------

VRWidgetsInteractor::VRWidgetsInteractor() throw() :
    m_pickedObject(nullptr),
    m_widget      (nullptr)
{
}

//------------------------------------------------------------------------------

VRWidgetsInteractor::~VRWidgetsInteractor() throw()
{
}

//------------------------------------------------------------------------------

Ogre::MovableObject *VRWidgetsInteractor::pickObject(int x, int y)
{
    ::Ogre::Camera *camera = m_sceneManager->getCamera("PlayerCam");

    int height = camera->getViewport()->getActualHeight();
    int width  = camera->getViewport()->getActualWidth();

    bool pickSuccess = m_picker.executeRaySceneQuery( x, y, width, height );

    return pickSuccess ? m_picker.getSelectedObject() : nullptr;
}

//------------------------------------------------------------------------------

void VRWidgetsInteractor::mouseMoveEvent(MouseButton button, int x, int y, int dx, int dy)
{
    if(button == LEFT)
    {
        if(m_pickedObject == nullptr)
        {
            TrackballInteractor::mouseMoveEvent(button, x, y, dx, dy);
        }
        else
        {
            m_widget->widgetPicked(m_pickedObject, x, y);
        }
    }
    else if(button == MIDDLE)
    {
        m_widget->moveClippingBox(x, y, -dx, -dy);
    }
    else if(button == RIGHT)
    {
        m_widget->scaleClippingBox(x, y, dy);
    }
}

//------------------------------------------------------------------------------

void VRWidgetsInteractor::buttonReleaseEvent(MouseButton button, int x, int y)
{
    m_widget->widgetReleased();
    m_pickedObject = nullptr;
}

//------------------------------------------------------------------------------

void VRWidgetsInteractor::buttonPressEvent(MouseButton button, int x, int y)
{
    if(button == LEFT)
    {
        m_pickedObject = pickObject(x, y);
        m_widget->widgetPicked(m_pickedObject, x, y);
    }
    else if(button == MIDDLE)
    {
        m_widget->moveClippingBox(x, y, 0, 0);
    }
    else if(button == RIGHT)
    {
        m_widget->scaleClippingBox(x, y, 0);
    }
}

//------------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre

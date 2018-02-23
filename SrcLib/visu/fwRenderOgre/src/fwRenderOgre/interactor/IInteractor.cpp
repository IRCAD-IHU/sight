/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

namespace fwRenderOgre
{
namespace interactor
{

const ::fwCom::Signals::SignalKeyType fwRenderOgre::interactor::IInteractor::s_POINT_CLICKED_SIG    = "pointClicked";
const ::fwCom::Signals::SignalKeyType fwRenderOgre::interactor::IInteractor::s_RENDER_REQUESTED_SIG = "renderRequested";

// ----------------------------------------------------------------------------

IInteractor::IInteractor()  :
    m_sceneManager(nullptr)
{
    m_ogreRoot = ::fwRenderOgre::Utils::getOgreRoot();

    m_sigRenderRequested = newSignal< RenderRequestedSigType >( s_RENDER_REQUESTED_SIG );

    m_sigPointClicked = newSignal< PointClickedSigType >( s_POINT_CLICKED_SIG );
}

// ----------------------------------------------------------------------------

IInteractor::~IInteractor()
{
}

// ----------------------------------------------------------------------------

void IInteractor::setSceneID(const std::string& sceneID)
{
    m_sceneManager = m_ogreRoot->getSceneManager(sceneID);
}

// ----------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre

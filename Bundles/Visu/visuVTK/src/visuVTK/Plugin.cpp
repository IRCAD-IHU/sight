/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>

#include <fwRender/IRender.hpp>
#include <fwRenderVTK/SRender.hpp>
#include <fwRenderVTK/vtk/Instantiator.hpp>

#include "visuVTK/Plugin.hpp"

namespace visuVTK
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("visuVTK::Plugin");


Plugin::~Plugin() noexcept
{
}

void Plugin::start()
{
    SLM_TRACE_FUNC();

    ::fwRenderVTK::vtk::Instantiator::ClassInitialize();
}

void Plugin::stop() noexcept
{
    ::fwRenderVTK::vtk::Instantiator::ClassFinalize();
}

} // namespace operators

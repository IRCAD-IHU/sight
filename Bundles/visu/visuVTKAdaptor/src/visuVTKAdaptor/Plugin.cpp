/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IQtQmlType.hpp>

namespace visuVTKAdaptor
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::visuVTKAdaptor::Plugin");

Plugin::~Plugin() noexcept
{
    ::fwServices::IQtQmlType::registarAllClasses();
}

//------------------------------------------------------------------------------

void Plugin::start()
{
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

} // namespace operators

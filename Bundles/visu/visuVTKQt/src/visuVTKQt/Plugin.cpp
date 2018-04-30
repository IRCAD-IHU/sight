/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKQt/Plugin.hpp"

#include <fwCore/spyLog.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <QApplication>
#include <QSurfaceFormat>
#include <QVTKOpenGLWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>

namespace visuVTKQt
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("visuVTKQt::Plugin");

//-----------------------------------------------------------------------------

Plugin::Plugin() noexcept
{
    // Since we share the opengl context with Qt, we must set the default QSurfaceFormat before QApplication
    // to allow QVTKOpenGLWidget to work properly
    SLM_ASSERT( "QApplication is already created! The default surface format cannot be changed afterward.\n"
                "This problem can happen when the profile.xml is handwritten and contains 'visuVTKQt' and 'guiQt'.\n"
                "If that is the case, please ensure that 'visuVTKQt' is started before 'guiQt'.\n"
                "It could also happen for generated 'profile.xml' files, check it to be sure.",
                qApp == nullptr );

/*    vtkOpenGLRenderWindow::SetGlobalMaximumNumberOfMultiSamples(0);
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());*/
}

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

} // namespace visuVTKQt

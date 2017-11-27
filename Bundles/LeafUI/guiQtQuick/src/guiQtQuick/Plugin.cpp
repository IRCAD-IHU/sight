/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "guiQtQuick/Plugin.hpp"

#include <fwCore/base.hpp>

#include <fwGuiQuick/App.hpp>
#include <fwGuiQuick/WorkerQt.hpp>

#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <QFile>
#include <QString>
#include <QTextStream>

#include <functional>

namespace guiQtQuick
{
//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::guiQtQuick::Plugin");

//------------------------------------------------------------------------------

Plugin::~Plugin() throw()
{
}

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    SLM_ASSERT("Profile is not initialized", profile);
    int& argc   = profile->getRawArgCount();
    char** argv = profile->getRawParams();

    m_workerQt = ::fwGuiQuick::getQtWorker(argc, argv);

    ::fwServices::registry::ActiveWorkers::setDefaultWorker(m_workerQt);

    m_workerQt->post( std::bind( &Plugin::loadStyleSheet, this ) );

    ::fwRuntime::profile::getCurrentProfile()->setRunCallback(std::bind(&Plugin::run, this));
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{
}

//-----------------------------------------------------------------------------

void setup()
{
    ::fwRuntime::profile::getCurrentProfile()->setup();
}

//-----------------------------------------------------------------------------

int Plugin::run() throw()
{
    m_workerQt->post( std::bind( &setup ) );
    m_workerQt->getFuture().wait(); // This is required to start WorkerQt loop

    ::fwRuntime::profile::getCurrentProfile()->cleanup();
    int result = ::boost::any_cast<int>(m_workerQt->getFuture().get());

    ::fwServices::registry::ActiveWorkers::getDefault()->clearRegistry();
    m_workerQt.reset();

    return result;
}

//-----------------------------------------------------------------------------

void Plugin::loadStyleSheet()
{
    if( this->getBundle()->hasParameter("style") )
    {
        std::string styleFile = this->getBundle()->getParameterValue("style");

        QFile data(QString::fromStdString(styleFile));
        QString style;
        if(data.open(QFile::ReadOnly))
        {
            QTextStream styleIn(&data);
            style = styleIn.readAll();
            data.close();
            qApp->setStyleSheet(style);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace guiQtQuick

/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "guiQt/Plugin.hpp"

#include <fwCore/base.hpp>

#include <fwGuiQt/App.hpp>

#include <fwQt/WorkerQt.hpp>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <QFile>
#include <QResource>
#include <QString>
#include <QStyleFactory>
#include <QTextStream>

#include <functional>

namespace guiQt
{
//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::guiQt::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    SLM_ASSERT("Profile is not initialized", profile);
    int& argc   = profile->getRawArgCount();
    char** argv = profile->getRawParams();

    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback
        = [](int& argc, char** argv)
          {
              return QSharedPointer< QApplication > ( new ::fwGuiQt::App(argc, argv, true) );
          };

    m_workerQt = ::fwQt::getQtWorker(argc, argv, callback, profile->getName(), profile->getVersion());

    ::fwServices::registry::ActiveWorkers::setDefaultWorker(m_workerQt);

    m_workerQt->post( std::bind( &Plugin::loadStyleSheet, this ) );

    ::fwRuntime::profile::getCurrentProfile()->setRunCallback(std::bind(&Plugin::run, this));
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    if(m_workerQt)
    {
        m_workerQt->stop();
        m_workerQt.reset();
    }
}

//-----------------------------------------------------------------------------

void setup()
{
    ::fwRuntime::profile::getCurrentProfile()->setup();
}

//-----------------------------------------------------------------------------

int Plugin::run() noexcept
{
    m_workerQt->post( std::bind( &setup ) );
    m_workerQt->getFuture().wait(); // This is required to start WorkerQt loop

    ::fwRuntime::profile::getCurrentProfile()->cleanup();
    int result = std::any_cast<int>(m_workerQt->getFuture().get());

    ::fwServices::registry::ActiveWorkers::getDefault()->clearRegistry();
    m_workerQt.reset();

    return result;
}

//-----------------------------------------------------------------------------

void Plugin::loadStyleSheet()
{
    if( this->getModule()->hasParameter("resource") )
    {
        const std::string resourceFile = this->getModule()->getParameterValue("resource");
        const auto path                = fwRuntime::getModuleResourceFilePath(resourceFile);

        const bool resourceLoaded = QResource::registerResource(path.string().c_str());
        SLM_ASSERT("Cannot load resources '"+resourceFile+"'.", resourceLoaded);
    }

    if( this->getModule()->hasParameter("style") )
    {
        const std::string style = this->getModule()->getParameterValue("style");
        qApp->setStyle(QStyleFactory::create(QString::fromStdString(style)));
    }

    if( this->getModule()->hasParameter("stylesheet") )
    {
        const std::string stylesheetFile = this->getModule()->getParameterValue("stylesheet");
        const auto path                  = fwRuntime::getModuleResourceFilePath(stylesheetFile);

        QFile data(QString::fromStdString(path.string()));
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

} // namespace guiQt

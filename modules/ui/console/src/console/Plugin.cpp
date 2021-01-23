/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "console/Plugin.hpp"

#include <core/runtime/profile/Profile.hpp>
#include <core/runtime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>

namespace console
{
//-----------------------------------------------------------------------------

static core::runtime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::console::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    m_worker = core::thread::Worker::New();
    ::fwServices::registry::ActiveWorkers::setDefaultWorker(m_worker);

    core::runtime::profile::getCurrentProfile()->setRunCallback(std::bind(&Plugin::run, this));
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    if(m_worker)
    {
        m_worker->stop();
    }
}

//-----------------------------------------------------------------------------

int Plugin::run() noexcept
{
    m_worker->post( []() {core::runtime::profile::getCurrentProfile()->setup(); } );
    m_worker->getFuture().wait(); // This is required to start WorkerAsio loop

    core::runtime::profile::getCurrentProfile()->cleanup();
    const std::uint64_t result = std::any_cast<std::uint64_t>(m_worker->getFuture().get());

    ::fwServices::registry::ActiveWorkers::getDefault()->clearRegistry();
    m_worker.reset();

    return result;
}

//-----------------------------------------------------------------------------

} // namespace console

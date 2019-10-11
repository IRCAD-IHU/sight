/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwRuntime/ExecutableFactoryRegistrar.hpp"

#include "fwRuntime/impl/Bundle.hpp"
#include "fwRuntime/impl/Runtime.hpp"

namespace fwRuntime
{

ExecutableFactoryRegistrar::ExecutableFactoryRegistrar( std::shared_ptr< ExecutableFactory > factory )
{
    // Pre-condition
    SLM_ASSERT("No bundle bundle currently loaded", impl::Bundle::getLoadingBundle() != nullptr);

    // Retrieves the bundle that is currently loading.
    std::shared_ptr< impl::Bundle >  loadingBundle( impl::Bundle::getLoadingBundle() );

    // Stores the factory into that bundle and the default runtime instance.
    loadingBundle->addExecutableFactory( factory );

    impl::Runtime& runtime = impl::Runtime::get();
    runtime.addExecutableFactory( factory );
}

} // namespace fwRuntime

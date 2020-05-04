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

#pragma once

#if defined(linux) || defined(__linux) || defined (__APPLE__)

#include "fwRuntime/impl/dl/Native.hpp"

#include <dlfcn.h>

namespace fwRuntime
{

namespace impl
{
namespace dl
{

/**
 * @brief   Implements a posix native module.
 * @struct  Posix
 */
struct Posix : public Native
{

    /**
     * @brief       Constructor.
     *
     * @param[in]   modulePath  a path to the module to manage
     */
    Posix( const std::filesystem::path& modulePath ) noexcept;

    /**
     * @brief   Destructor.
     */
    ~Posix() noexcept override;

    /**
     * @see ::fwRuntime::dl::Native#isLoaded
     */
    bool isLoaded() const noexcept override;

    /**
     * @brief       Retrieves the address of a symbol specified by its name.
     *
     * @param[in]   name    a string containing the symbol name.
     *
     * @return      a pointer to the found symbol or null if none has been found
     */
    void* getSymbol( const std::string& name ) const override;

    /**
     * @see ::fwRuntime::dl::Native#load
     */
    void load() override;

    /**
     * @see ::fwRuntime::dl::Native#unload
     */
    void unload() override;

    private:

        /**
         * @brief   The handle of the loaded module.
         */
        void* m_handle;

};

} // namespace dl

} // namespace impl

} // namespace fwRuntime

#endif // #ifdef linux

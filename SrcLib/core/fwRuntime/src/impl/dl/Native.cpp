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

#include "fwRuntime/impl/dl/Native.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/Runtime.hpp"

#include <limits.h>

#include <filesystem>
#include <string>

namespace fwRuntime
{

namespace impl
{

namespace dl
{

//------------------------------------------------------------------------------

Native::Native( const std::filesystem::path& modulePath ) noexcept :
    m_modulePath( modulePath ),
    m_bundle( 0 )
{
}

//------------------------------------------------------------------------------

Native::~Native() noexcept
{
}

//------------------------------------------------------------------------------

const std::filesystem::path Native::getBundleLocation() const
{
    return m_bundle->getLibraryLocation();
}

//------------------------------------------------------------------------------

const std::filesystem::path Native::getFullPath( const bool _bMustBeFile ) const
{
    // Pre-condition
    SLM_ASSERT("bundle not initialized", m_bundle != 0 );

    std::filesystem::path result;

    result = this->getBundleLocation() / this->getPath();

    // Test that the result path exists.
    if(result.empty())
    {
        throw RuntimeException("Unable to find a native library for the bundle.");
    }
    if( !std::filesystem::exists(result) )
    {
        throw RuntimeException("'" + result.string() + "': invalid native module file name.");
    }
    if(_bMustBeFile && std::filesystem::is_directory(result) )
    {
        throw RuntimeException("'" + result.string() + "': is a directory. Perhaps dynamic library is missing.");
    }
    return result;
}

//------------------------------------------------------------------------------

const std::regex Native::getNativeName() const
{
    const std::filesystem::path fullModulePath( this->getBundleLocation() / m_modulePath );
    std::regex nativeName;

#if defined(linux) || defined(__linux)
    nativeName = std::regex(
        "lib" + fullModulePath.filename().string() + "\\.so" +
        "[0-9\\.]*" );
#elif defined(WIN32)
    nativeName = std::regex(
        fullModulePath.filename().string() + "\\.dll");
#elif defined (__APPLE__)
    nativeName = std::regex(
        "lib" + fullModulePath.filename().string() + "[0-9\\.]*\\.dylib" );
#endif

    return nativeName;
}

//------------------------------------------------------------------------------

const std::filesystem::path Native::getPath() const
{
    // Pre-condition
    SLM_ASSERT("bundle not initialized", m_bundle != 0 );

    std::filesystem::path result;

    const std::filesystem::path fullModulePath( this->getBundleLocation() / m_modulePath );
    const std::regex nativeFileRegex( this->getNativeName() );

    // Walk through the module directory, seeking for a matching file.
    std::filesystem::directory_iterator curDirEntry(fullModulePath.parent_path());
    std::filesystem::directory_iterator endDirEntry;
    for(; curDirEntry != endDirEntry; ++curDirEntry)
    {
        std::filesystem::path curEntryPath( *curDirEntry );
        if( std::regex_match( curEntryPath.filename().string(), nativeFileRegex ) )
        {
            result = m_modulePath.parent_path() / curEntryPath.filename();
            break;
        }
    }

    return result;
}

//------------------------------------------------------------------------------

void Native::setBundle( const Bundle* bundle ) noexcept
{
    // Pre-condition
    SLM_ASSERT("bundle already initialized", m_bundle == 0 );
    m_bundle = bundle;
    // Post-condition
    SLM_ASSERT("bundle not correctly attached", m_bundle == bundle );
}

//------------------------------------------------------------------------------

void Native::operator=(const Native&) noexcept
{
}

//------------------------------------------------------------------------------

} // namespace dl

} // namespace impl

} // namespace fwRuntime

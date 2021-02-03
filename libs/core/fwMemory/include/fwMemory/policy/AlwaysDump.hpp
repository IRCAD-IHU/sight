/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "fwMemory/BufferInfo.hpp"
#include "fwMemory/config.hpp"
#include "fwMemory/IPolicy.hpp"
#include "fwMemory/policy/factory/new.hpp"

#include <core/base.hpp>

namespace fwMemory
{
namespace policy
{

/**
 * @brief Always dump policy
 *
 * This policy will dump buffers as often as possible. As soon as a buffer is
 * released (ie the lock count become zero), it will be dumped.
 */
class FWMEMORY_CLASS_API AlwaysDump : public fwMemory::IPolicy
{
public:
    fwCoreClassMacro(AlwaysDump, fwMemory::IPolicy, ::fwMemory::policy::factory::New< AlwaysDump >)

    FWMEMORY_API virtual void allocationRequest( BufferInfo& info,
                                                 ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                                 BufferInfo::SizeType size ) override;

    FWMEMORY_API virtual void setRequest( BufferInfo& info,
                                          ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                          BufferInfo::SizeType size ) override;

    FWMEMORY_API virtual void reallocateRequest( BufferInfo& info,
                                                 ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                                 BufferInfo::SizeType newSize ) override;

    FWMEMORY_API virtual void destroyRequest( BufferInfo& info,
                                              ::fwMemory::BufferManager::ConstBufferPtrType buffer ) override;

    FWMEMORY_API virtual void lockRequest( BufferInfo& info,
                                           ::fwMemory::BufferManager::ConstBufferPtrType buffer ) override;
    FWMEMORY_API virtual void unlockRequest( BufferInfo& info,
                                             ::fwMemory::BufferManager::ConstBufferPtrType buffer ) override;

    FWMEMORY_API virtual void dumpSuccess( BufferInfo& info,
                                           ::fwMemory::BufferManager::ConstBufferPtrType buffer ) override;
    FWMEMORY_API virtual void restoreSuccess( BufferInfo& info,
                                              ::fwMemory::BufferManager::ConstBufferPtrType buffer ) override;

    FWMEMORY_API virtual void refresh() override;

    //------------------------------------------------------------------------------

    bool setParam(const std::string& name, const std::string& value) override
    {
        FwCoreNotUsedMacro(name);
        FwCoreNotUsedMacro(value);
        return false;
    }

    FWMEMORY_API virtual std::string getParam(const std::string& name, bool* ok = NULL ) const override;

    //------------------------------------------------------------------------------

    const ::fwMemory::IPolicy::ParamNamesType& getParamNames() const override
    {
        static ::fwMemory::IPolicy::ParamNamesType names;
        return names;
    }

protected:

    FWMEMORY_API size_t dump();
    FWMEMORY_API void apply();
};

} // namespace policy

} // namespace fwMemory

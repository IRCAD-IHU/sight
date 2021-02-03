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

#include <services/macros.hpp>

#include <core/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::services
{
namespace ut
{

/**
 * @brief   Service type for test.
 */
class TestConfigService : public services::IService
{
public:
    fwCoreServiceMacro(TestConfigService, services::IService)
    TestConfigService() noexcept :
        m_isUpdated(false)
    {
    }

    virtual ~TestConfigService() noexcept
    {
    }

    /// return true if the service is updated with updating() method
    bool getIsUpdated()
    {
        return m_isUpdated;
    }

protected:
    //------------------------------------------------------------------------------

    virtual void configuring() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void starting() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void stopping() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void updating() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void info( std::ostream& _sstream ) override
    {
        _sstream << "TestConfigService";
    }

    bool m_isUpdated;
};

/**
 * @brief   Test service implementation for image
 */
class TestServiceImplementationImage : public TestConfigService
{

public:
    fwCoreServiceMacro(TestServiceImplementationImage, services::ut::TestConfigService)
    TestServiceImplementationImage() noexcept
    {
    }
    virtual ~TestServiceImplementationImage() noexcept
    {
    }

    //------------------------------------------------------------------------------

    virtual void configuring() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void starting() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void stopping() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void updating() override
    {
        m_isUpdated = true;
    }

};

} //namespace ut
} //namespace sight::services

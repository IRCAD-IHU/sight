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

#include "AppConfigParametersTest.hpp"

#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/AppConfigParameters.hpp>

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/operations.hpp>
#include <fwRuntime/Runtime.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::AppConfigParametersTest );

namespace fwServices
{
namespace ut
{

//------------------------------------------------------------------------------

void AppConfigParametersTest::setUp()
{
    // Set up context before running a test.
    //modules location
    ::fwRuntime::Runtime* runtime = ::fwRuntime::Runtime::getDefault();
    runtime->addDefaultBundles();

    std::filesystem::path location = ::fwRuntime::getResourceFilePath("tu_exec_fwServices-0.0");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    runtime->addModules(location);

    ::fwRuntime::loadModule("servicesReg");
    ::fwRuntime::loadModule("AppConfigParametersTest");

    ::fwServices::registry::AppConfigParameters::sptr appConfigParam;
    appConfigParam = ::fwServices::registry::AppConfigParameters::getDefault();
    appConfigParam->parseBundleInformation();
}

//------------------------------------------------------------------------------

void AppConfigParametersTest::tearDown()
{
    // Clean up after the test run.
    ::fwServices::registry::AppConfigParameters::sptr appConfigParameters;
    appConfigParameters = ::fwServices::registry::AppConfigParameters::getDefault();
    appConfigParameters->clearRegistry();
}

//------------------------------------------------------------------------------

void AppConfigParametersTest::appConfigParametersTest()
{
    ::fwServices::registry::AppConfigParameters::sptr appConfigParam;
    appConfigParam = ::fwServices::registry::AppConfigParameters::getDefault();

    ::fwServices::registry::FieldAdaptorType parameters = appConfigParam->getParameters("id1");
    CPPUNIT_ASSERT(!parameters.empty());
    CPPUNIT_ASSERT_EQUAL(std::string("value1"), parameters["param1"]);

    ::fwServices::registry::FieldAdaptorType parameters2 = appConfigParam->getParameters("id2");
    CPPUNIT_ASSERT(!parameters2.empty());
    CPPUNIT_ASSERT_EQUAL(std::string("value3"), parameters2["param3"]);
}

//------------------------------------------------------------------------------

void AppConfigParametersTest::concurentAccessToAppConfigParametersTest()
{
    const auto fn = std::bind(&AppConfigParametersTest::appConfigParametersTest, this);
    std::vector< std::future<void> > futures;
    for (unsigned int i = 0; i < 20; ++i)
    {
        futures.push_back( std::async(std::launch::async, fn) );
    }

    for (auto& future : futures)
    {
        const auto status = future.wait_for(std::chrono::seconds(1));
        CPPUNIT_ASSERT(status == std::future_status::ready);
        future.get(); // Trigger exceptions
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices

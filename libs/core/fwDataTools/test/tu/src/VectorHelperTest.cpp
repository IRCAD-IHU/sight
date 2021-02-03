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

#include "VectorHelperTest.hpp"

#include <fwDataTools/helper/Vector.hpp>

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>

#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataTools::ut::VectorHelperTest );

namespace fwDataTools
{
namespace ut
{

//------------------------------------------------------------------------------

void VectorHelperTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void VectorHelperTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void VectorHelperTest::testHelper()
{
    ::fwData::Object::sptr nullobj;
    ::fwData::Vector::sptr vector     = ::fwData::Vector::New();
    ::fwData::Object::sptr vectorObj1 = ::fwData::String::New();
    ::fwData::Object::sptr vectorObj2 = ::fwData::String::New();
    ::fwData::Object::sptr vectorObj3 = ::fwData::String::New();

    core::thread::Worker::sptr worker = core::thread::Worker::New();

    // Setup to test notifications
    unsigned int numAddedNotif = 0;
    std::mutex mutex;
    std::condition_variable condition;

    ::fwData::Vector::ContainerType addedVectors;
    std::function<void ( ::fwData::Vector::ContainerType)> fnAdd =
        [&](::fwData::Vector::ContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numAddedNotif;
                addedVectors = f;
            }
            condition.notify_one();
        };

    auto slotAdded = core::com::newSlot( fnAdd );
    slotAdded->setWorker(worker);
    auto sigAdded = vector->signal< ::fwData::Vector::AddedObjectsSignalType>(::fwData::Vector::s_ADDED_OBJECTS_SIG);
    sigAdded->connect(slotAdded);

    unsigned int numRemovedNotif = 0;
    ::fwData::Vector::ContainerType removedVectors;

    std::function<void ( ::fwData::Vector::ContainerType)> fnRemove =
        [&](::fwData::Vector::ContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numRemovedNotif;
                removedVectors = f;
            }
            condition.notify_one();
        };
    auto slotRemoved = core::com::newSlot( fnRemove );
    slotRemoved->setWorker(worker);
    auto sigRemoved = vector->signal< ::fwData::Vector::RemovedObjectsSignalType>(
        ::fwData::Vector::s_REMOVED_OBJECTS_SIG);
    sigRemoved->connect(slotRemoved);

    auto clearArrays = [&]() { addedVectors.clear(); removedVectors.clear(); };

    {
        // Test add()
        ::fwDataTools::helper::Vector vectorHelper(vector);
        vectorHelper.add(vectorObj1);
        vectorHelper.add(vectorObj2);
        CPPUNIT_ASSERT_EQUAL(size_t(2), vector->size());
        CPPUNIT_ASSERT((*vector)[0] == vectorObj1);
        CPPUNIT_ASSERT((*vector)[1] == vectorObj2);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numAddedNotif == 1; });

        CPPUNIT_ASSERT_EQUAL(size_t(2), addedVectors.size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), removedVectors.size());
        CPPUNIT_ASSERT(addedVectors[0] == vectorObj1);
        CPPUNIT_ASSERT(addedVectors[1] == vectorObj2);
        clearArrays();
    }

    {
        // Test add()
        ::fwDataTools::helper::Vector vectorHelper(vector);
        vectorHelper.add(vectorObj3);
        CPPUNIT_ASSERT_EQUAL(size_t(3), vector->size());
        CPPUNIT_ASSERT((*vector)[0] == vectorObj1);
        CPPUNIT_ASSERT((*vector)[1] == vectorObj2);
        CPPUNIT_ASSERT((*vector)[2] == vectorObj3);

        // Notify explicitly, this should change nothing since the destructor skip it in this case
        vectorHelper.notify();
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numAddedNotif == 2; });

        CPPUNIT_ASSERT_EQUAL(size_t(1), addedVectors.size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), removedVectors.size());
        CPPUNIT_ASSERT(addedVectors[0] == vectorObj3);
        clearArrays();
    }

    {
        // Test remove()
        ::fwDataTools::helper::Vector vectorHelper(vector);
        vectorHelper.remove(vectorObj1);
        vectorHelper.remove(vectorObj2);
        CPPUNIT_ASSERT_EQUAL(size_t(1), vector->size());
        CPPUNIT_ASSERT((*vector)[0] == vectorObj3);

        // Notify explicitly, this should change nothing since the destructor skip it in this case
        vectorHelper.notify();
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numRemovedNotif == 1; });

        CPPUNIT_ASSERT_EQUAL(size_t(0), addedVectors.size());
        CPPUNIT_ASSERT_EQUAL(size_t(2), removedVectors.size());
        CPPUNIT_ASSERT(removedVectors[0] == vectorObj1);
        CPPUNIT_ASSERT(removedVectors[1] == vectorObj2);
        clearArrays();
    }

    {
        // Test add(), and clear()
        ::fwDataTools::helper::Vector vectorHelper(vector);
        vectorHelper.add(vectorObj2);
        CPPUNIT_ASSERT_EQUAL(size_t(2), vector->size());
        CPPUNIT_ASSERT((*vector)[0] == vectorObj3);
        CPPUNIT_ASSERT((*vector)[1] == vectorObj2);

        vectorHelper.clear();
        CPPUNIT_ASSERT_EQUAL(size_t(0), vector->size());
        CPPUNIT_ASSERT_EQUAL(true, vector->empty());
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numAddedNotif == 3 && numRemovedNotif == 2; });

        CPPUNIT_ASSERT_EQUAL(size_t(1), addedVectors.size());
        CPPUNIT_ASSERT_EQUAL(size_t(2), removedVectors.size());
        CPPUNIT_ASSERT(addedVectors[0] == vectorObj2);
        CPPUNIT_ASSERT(removedVectors[0] == vectorObj3);
        CPPUNIT_ASSERT(removedVectors[1] == vectorObj2);
        clearArrays();
    }

    worker->stop();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataTools

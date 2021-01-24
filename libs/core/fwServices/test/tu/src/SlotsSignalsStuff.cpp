/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "SlotsSignalsStuff.hpp"

#include <fwServices/macros.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <core/com/Connection.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/mt/ObjectWriteLock.hpp>
#include <data/registry/macros.hpp>

namespace fwServices
{
namespace ut
{

fwDataRegisterMacro(Buffer);

fwServicesRegisterMacro( ::fwServices::ut::IBasicTest, ::fwServices::ut::SBasicTest )
fwServicesRegisterObjectMacro( ::fwServices::ut::SBasicTest, ::fwServices::ut::Buffer );
fwServicesRegisterMacro( ::fwServices::ut::IBasicTest, ::fwServices::ut::SReaderTest )
fwServicesRegisterObjectMacro( ::fwServices::ut::SReaderTest, ::fwServices::ut::Buffer );
fwServicesRegisterMacro( ::fwServices::ut::IBasicTest, ::fwServices::ut::SShowTest )
fwServicesRegisterObjectMacro( ::fwServices::ut::SShowTest, ::fwServices::ut::Buffer );
fwServicesRegisterMacro( ::fwServices::ut::IBasicTest, ::fwServices::ut::SReader2Test )
fwServicesRegisterObjectMacro( ::fwServices::ut::SReader2Test, ::fwServices::ut::Buffer );
fwServicesRegisterMacro( ::fwServices::ut::IBasicTest, ::fwServices::ut::SShow2Test )
fwServicesRegisterObjectMacro( ::fwServices::ut::SShow2Test, ::fwServices::ut::Buffer );

const ::fwServices::IService::KeyType IBasicTest::s_BUFFER_INOUT = "buffer";

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

SBasicTest::SBasicTest() :
    m_updateFinished(false),
    m_swapFinished(false)
{
}

//------------------------------------------------------------------------------

void SBasicTest::starting()
{
    std::this_thread::sleep_for(m_startRetarder);
}

//------------------------------------------------------------------------------

void SBasicTest::stopping()
{
    std::this_thread::sleep_for(m_stopRetarder);
}

//------------------------------------------------------------------------------

void SBasicTest::swapping(const KeyType& )
{
    std::this_thread::sleep_for(m_swapRetarder);
    m_swapFinished = true;
}

//------------------------------------------------------------------------------

void SBasicTest::updating()
{
    std::this_thread::sleep_for(m_updateRetarder);
    m_updateFinished = true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void SReaderTest::updating()
{
    Buffer::sptr buff = this->getInOut< Buffer >(s_BUFFER_INOUT);

    // Emit object Modified
    data::Object::ModifiedSignalType::sptr sig;
    sig = buff->signal< data::Object::ModifiedSignalType >( data::Object::s_MODIFIED_SIG );

    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const core::com::Slots::SlotKeyType SShowTest::s_CHANGE_SLOT = "change";

//------------------------------------------------------------------------------

SShowTest::SShowTest() :
    m_receiveCount(0),
    m_changeCount(0)
{
    m_slotChange = core::com::newSlot( &SShowTest::change, this );
    core::com::HasSlots::m_slots( s_CHANGE_SLOT, m_slotChange );
}

//------------------------------------------------------------------------------

void SShowTest::updating()
{
    Buffer::sptr buffer = this->getInOut<Buffer>(s_BUFFER_INOUT);
    std::this_thread::sleep_for(m_receiveRetarder);
    data::mt::ObjectWriteLock lock(buffer);
    ++m_receiveCount;
}

//------------------------------------------------------------------------------

void SShowTest::change()
{
    core::mt::ScopedLock lock(m_mutex);
    ++m_changeCount;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SShowTest::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_BUFFER_INOUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const core::com::Signals::SignalKeyType SReader2Test::s_CHANGED_SIG = "changed";

//------------------------------------------------------------------------------

SReader2Test::SReader2Test()
{
    m_sigChanged = ChangedSignalType::New();
    // Register
    core::com::HasSignals::m_signals( s_CHANGED_SIG,  m_sigChanged);
}

//------------------------------------------------------------------------------

void SReader2Test::updating()
{
    // Emit object Modified
    SReader2Test::ChangedSignalType::sptr sig;
    sig = this->signal< SReader2Test::ChangedSignalType >( SReader2Test::s_CHANGED_SIG );
    sig->asyncEmit();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const core::com::Slots::SlotKeyType SShow2Test::s_UPDATE_BUFFER_SLOT = "updateBuffer";

//------------------------------------------------------------------------------

SShow2Test::SShow2Test() :
    m_receiveCount(0)
{
    newSlot(s_UPDATE_BUFFER_SLOT, &SShow2Test::updateBuffer, this);
}

//------------------------------------------------------------------------------

void SShow2Test::updating()
{
    Buffer::sptr buff = this->getInOut< Buffer >(s_BUFFER_INOUT);

    // Emit object Modified
    data::Object::ModifiedSignalType::sptr sig;
    sig = buff->signal< data::Object::ModifiedSignalType >( data::Object::s_MODIFIED_SIG );
    {
        core::com::Connection::Blocker block(sig->getConnection(this->slot(s_UPDATE_BUFFER_SLOT)));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SShow2Test::updateBuffer()
{
    {
        Buffer::sptr buffer = this->getInOut<Buffer>(s_BUFFER_INOUT);
        std::this_thread::sleep_for(m_receiveRetarder);
        data::mt::ObjectWriteLock lock(buffer);
        ++m_receiveCount;
    }

    this->updating();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices

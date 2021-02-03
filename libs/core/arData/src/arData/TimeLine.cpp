/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "arData/TimeLine.hpp"

#include <core/com/Signal.hxx>

namespace arData
{

const core::com::Signals::SignalKeyType TimeLine::s_OBJECT_PUSHED_SIG  = "objectPushed";
const core::com::Signals::SignalKeyType TimeLine::s_OBJECT_REMOVED_SIG = "objectRemoved";
const core::com::Signals::SignalKeyType TimeLine::s_CLEARED_SIG        = "objectCleared";

//------------------------------------------------------------------------------

TimeLine::TimeLine ( ::fwData::Object::Key )
{
    newSignal<ObjectPushedSignalType>(s_OBJECT_PUSHED_SIG);
    newSignal<ObjectRemovedSignalType>(s_OBJECT_REMOVED_SIG);
    newSignal<ObjectClearedSignalType>(s_CLEARED_SIG);
}

//------------------------------------------------------------------------------

TimeLine::~TimeLine ()
{
}

//------------------------------------------------------------------------------

} // namespace arData

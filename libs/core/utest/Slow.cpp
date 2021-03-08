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

#include "utest/Slow.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace sight::utest
{

//------------------------------------------------------------------------------

static bool getIgnoreSlowTestsEnv()
{
    const char* slowTests = std::getenv( "FW_IGNORE_SLOW_TESTS" );
    if(slowTests == nullptr || std::strlen(slowTests) == 0 || std::strcmp(slowTests, "0") == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

//------------------------------------------------------------------------------

bool Slow::ignoreSlowTests()
{
    static const bool IGNORE_SLOW_TESTS = getIgnoreSlowTestsEnv();
    return IGNORE_SLOW_TESTS;
}

} // namespace sight::utest

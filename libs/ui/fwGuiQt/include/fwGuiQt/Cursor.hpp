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

#include "fwGuiQt/config.hpp"

#include <core/base.hpp>

#include <fwGui/ICursor.hpp>

namespace fwGuiQt
{

/**
 * @brief   Defines the generic cursor for IHM.
 */
class FWGUIQT_CLASS_API Cursor : public ::fwGui::ICursor
{

public:

    fwCoreClassMacro(Cursor, ::fwGui::ICursor, ::fwGui::factory::New< Cursor >)

    FWGUIQT_API Cursor(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~Cursor();

    /// Set the cursor
    FWGUIQT_API virtual void setCursor( ::fwGui::ICursor::CursorType cursor) override;

    /// Set the default cursor
    FWGUIQT_API virtual void setDefaultCursor() override;

};

} // namespace fwGuiQt

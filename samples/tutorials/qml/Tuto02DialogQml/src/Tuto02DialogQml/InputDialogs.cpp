/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "Tuto02DialogQml/InputDialogs.hpp"

#include <fwGui/dialog/InputDialog.hpp>

#include <fwQml/QmlEngine.hpp>

#include <QGuiApplication>

InputDialogs::InputDialogs()
{
}

//------------------------------------------------------------------------------

InputDialogs::~InputDialogs()
{
}

//------------------------------------------------------------------------------

void InputDialogs::open()
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    const std::string& result = ::fwGui::dialog::InputDialog::showInputDialog(
        m_title.toStdString(), m_message.toStdString(), m_input.toStdString());

    m_result = QString::fromStdString(result);
    Q_EMIT onResultChanged();
}

/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "PoCDialog/PulseProgressDialogs.hpp"

#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/assign/list_of.hpp>

#include <QGuiApplication>
#include <QMessageBox>
#include <QObject>

PulseProgressDialogs::PulseProgressDialogs()
{
    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    // add context for root Context
    engine->getRootContext()->setContextProperty("pocPulseDialogProgressDialogs", this);
}

//------------------------------------------------------------------------------

PulseProgressDialogs::~PulseProgressDialogs()
{
}

//------------------------------------------------------------------------------

void poCTest()
{
    int i = 0;
    while (i++ < 100)
    {
        boost::this_thread::sleep( boost::posix_time::seconds(1) );
    }
}

//------------------------------------------------------------------------------

void PulseProgressDialogs::open()
{
    std::function<void()> test = poCTest;
    ::fwGui::dialog::PulseProgressDialog dialog(m_title.toStdString(), test, m_message.toStdString());
    dialog.show();
}

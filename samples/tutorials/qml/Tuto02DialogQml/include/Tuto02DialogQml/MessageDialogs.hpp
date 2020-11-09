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

#pragma once

#include "Tuto02DialogQml/config.hpp"

#include <QObject>

/**
 * @brief   This class is associated to MessageDialogs.qml and calls
 * MessageDialog with the selected options
 */
class TUTO02DIALOGQML_CLASS_API MessageDialogs : public QObject
{
Q_OBJECT
Q_PROPERTY(int icon MEMBER m_icon)
Q_PROPERTY(QString message MEMBER m_message)
Q_PROPERTY(QString title MEMBER m_title)
Q_PROPERTY(int buttons MEMBER m_buttons)
Q_PROPERTY(QString result MEMBER m_result NOTIFY onResultChanged)

public:

    TUTO02DIALOGQML_API MessageDialogs();

    TUTO02DIALOGQML_API virtual ~MessageDialogs() override;

Q_SIGNALS:
    void onResultChanged();

protected Q_SLOTS:
    void open();

private:
    /// Dialog title
    QString m_title;
    /// Dialog box message
    QString m_message;
    /// Text inputed
    QString m_input;
    /// icon of the message dialog
    int m_icon {0};
    /// the buttons to add to the dialog
    int m_buttons {0};
    QString m_result;
};

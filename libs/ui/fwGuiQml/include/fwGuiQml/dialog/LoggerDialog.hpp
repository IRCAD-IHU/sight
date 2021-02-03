/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#pragma once

#include "fwGuiQml/config.hpp"

#include <QObject>
#include <QUrl>

#include <ui/base/dialog/ILoggerDialog.hpp>

#include <vector>

namespace fwGuiQml
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   LoggerDialog allowing the choice of an element among severals (_selections)
 */
class FWGUIQML_CLASS_API LoggerDialog : public QObject,
                                        public ui::base::dialog::ILoggerDialog
{
Q_OBJECT
Q_PROPERTY(QUrl hidden MEMBER m_hidden WRITE emitHidden NOTIFY hiddenChanged)
Q_PROPERTY(QUrl icon MEMBER m_icon WRITE emitIcon NOTIFY iconChanged)
Q_PROPERTY(QString message MEMBER m_message WRITE emitMessage NOTIFY messageChanged)
Q_PROPERTY(QUrl shown MEMBER m_shown WRITE emitShown NOTIFY shownChanged)

public:

    fwCoreClassMacro(LoggerDialog, ui::base::dialog::ILoggerDialog, ui::base::factory::New< LoggerDialog >)

    /// Constructor
    FWGUIQML_API LoggerDialog(ui::base::GuiBaseObject::Key key);

    /// Destructor
    FWGUIQML_API virtual ~LoggerDialog();

    /**
     * @brief Set the dialog title.
     * @param[in] title Dialog title
     */
    FWGUIQML_API virtual void setTitle(const std::string& title) override;

    /**
     * @brief Set the dialog message.
     * @param[in] message Dialog message
     */
    FWGUIQML_API virtual void setMessage(const std::string& message) override;

    /**
     * @brief Set the dialog logger.
     * @param[in] logger Dialog logger
     */
    FWGUIQML_API virtual void setLogger(const core::log::Logger::sptr& logger) override;

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    FWGUIQML_API virtual bool show() override;

Q_SIGNALS:
    /// notify the qml of property change
    void hiddenChanged();
    void iconChanged();
    void messageChanged();
    void shownChanged();

protected Q_SLOTS:
    /// slot getting the result of the dialog when a button is pressed
    void resultDialog(bool isOk);

private:
    /// Dialog title
    QString m_title;

    /// Dialog message
    QString m_message;

    /// Logger
    core::log::Logger::sptr m_logger;

    /// get pushed button and clicked one
    bool m_isOk {false};

    /// Resume the biggest error get with an icon
    QUrl m_icon;

    /// Icons path of the logger
    QUrl m_hidden;
    QUrl m_shown;

    /// Setter to QProperty and emit signal
    FWGUIQML_API void emitHidden(const QUrl&);
    FWGUIQML_API void emitIcon(const QUrl&);
    FWGUIQML_API void emitMessage(const QString&);
    FWGUIQML_API void emitShown(const QUrl&);
};

} // namespace dialog
} // namespace fwGuiQml

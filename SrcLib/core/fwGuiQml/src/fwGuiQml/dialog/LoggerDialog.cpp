/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwGuiQml/dialog/LoggerDialog.hpp"

#include "fwGuiQml/model/RoleTableModel.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/foreach.hpp>

#include <QGuiApplication>
#include <Qt>

fwGuiRegisterMacro( ::fwGuiQml::dialog::LoggerDialog, ::fwGui::dialog::ILoggerDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

LoggerDialog::LoggerDialog(::fwGui::GuiBaseObject::Key key)
{
}

//------------------------------------------------------------------------------

LoggerDialog::~LoggerDialog()
{
}

//------------------------------------------------------------------------------

void LoggerDialog::setTitle(const std::string& title)
{
    m_title = QString::fromStdString(title);
}

//------------------------------------------------------------------------------

void LoggerDialog::setMessage(const std::string& message)
{
    m_message = QString::fromStdString(message);
}

//------------------------------------------------------------------------------

void LoggerDialog::setLogger(const ::fwLog::Logger::sptr& logger)
{
    m_logger = logger;
}

//------------------------------------------------------------------------------

bool LoggerDialog::show()
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    ::fwGuiQml::model::RoleTableModel model;

    // get the path of the qml ui file in the 'rc' directory
    auto dialogPath = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/dialog/LoggerDialog.qml");

    // load the qml ui component
    QObject* dialog = engine->createComponent(dialogPath);
    QObject::connect(dialog, SIGNAL(resultDialog(bool)),
                     this, SLOT(resultDialog(bool)));
    engine->getRootContext()->setContextProperty("loggerModel", &model);
    engine->getRootContext()->setContextProperty("loggerDialog", this);
    Q_EMIT titleChanged();

    if (m_logger->count(::fwLog::Log::CRITICAL) > 0)
    {
        const auto path = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/critical.png").string();
        emitIcon(QUrl::fromLocalFile(QString::fromStdString(path)));
    }
    else if (m_logger->count(::fwLog::Log::WARNING) > 0)
    {
        const auto path = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/warning.png").string();
        emitIcon(QUrl::fromLocalFile(QString::fromStdString(path)));
    }
    else
    {
        const auto path = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/information.png").string();
        emitIcon(QUrl::fromLocalFile(QString::fromStdString(path)));
    }
    // Create message
    std::stringstream ss;
    ss << m_message.toStdString() <<
        "<br><br>" << "<b>Log report :</b> " << m_logger->count(::fwLog::Log::CRITICAL) << " critical, " <<
        m_logger->count(::fwLog::Log::WARNING) << " warning and " <<
        m_logger->count(::fwLog::Log::INFORMATION) << " information messages.";
    emitMessage(QString::fromStdString(ss.str()));

    const auto detailshidden = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/details-hidden.png").string();
    const auto detailsshown  = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/details-shown.png").string();
    emitHidden(QUrl::fromLocalFile(QString::fromStdString(detailshidden)));
    emitShown(QUrl::fromLocalFile(QString::fromStdString(detailsshown)));

    const auto information = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/information.png").string();
    const auto warning     = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/warning.png").string();
    const auto critical    = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/critical.png").string();
    emitInformation(QUrl::fromLocalFile(QString::fromStdString(information)));
    emitWarning(QUrl::fromLocalFile(QString::fromStdString(warning)));
    emitCritical(QUrl::fromLocalFile(QString::fromStdString(critical)));

    // Fill log table
    ::fwLog::Logger::ConstIteratorType it = m_logger->begin();
    model.addRole(Qt::UserRole + 1, "level");
    model.addRole(Qt::UserRole + 2, "message");
    for(; it != m_logger->end(); ++it)
    {
        QString levelString = "Unkown";
        QHash<QByteArray, QVariant> data;
        ::fwLog::Log::LevelType level = it->getLevel();
        if (level == ::fwLog::Log::INFORMATION)
        {
            levelString = "Information";
        }
        else if (level == ::fwLog::Log::WARNING)
        {
            levelString = "Warning";
        }
        else if (level == ::fwLog::Log::CRITICAL)
        {
            levelString = "Critical";
        }
        data.insert("level", levelString);
        data.insert("message", QString::fromStdString(it->getMessage()));
        model.addData(QHash<QByteArray, QVariant>(data));
    }
    QMetaObject::invokeMethod(dialog, "open");

    while (!m_isClicked && m_visible)
    {
        qGuiApp->processEvents();
    }
    delete dialog;
    return m_isOk;
}

//------------------------------------------------------------------------------

void LoggerDialog::resultDialog(bool isOk)
{
    m_isOk      = isOk;
    m_isClicked = true;
}

//------------------------------------------------------------------------------

void LoggerDialog::emitCritical(const QUrl& critical)
{
    m_critical = critical;
    Q_EMIT criticalChanged();
}

//------------------------------------------------------------------------------

void LoggerDialog::emitHidden(const QUrl& hidden)
{
    m_hidden = hidden;
    Q_EMIT hiddenChanged();
}

//------------------------------------------------------------------------------

void LoggerDialog::emitIcon(const QUrl& path)
{
    m_icon = path;
    Q_EMIT iconChanged();
}

//------------------------------------------------------------------------------

void LoggerDialog::emitInformation(const QUrl& information)
{
    m_information = information;
    Q_EMIT informationChanged();
}

//------------------------------------------------------------------------------

void LoggerDialog::emitMessage(const QString& message)
{
    m_message = message;
    Q_EMIT messageChanged();
}

//------------------------------------------------------------------------------

void LoggerDialog::emitShown(const QUrl& shown)
{
    m_shown = shown;
    Q_EMIT shownChanged();
}

//------------------------------------------------------------------------------

void LoggerDialog::emitWarning(const QUrl& warning)
{
    m_warning = warning;
    Q_EMIT warningChanged();
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml

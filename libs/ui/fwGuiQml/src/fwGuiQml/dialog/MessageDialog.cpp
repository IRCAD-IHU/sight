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

#include "fwGuiQml/dialog/MessageDialog.hpp"

#include <core/runtime/operations.hpp>

#include <fwQml/QmlEngine.hpp>

#include <gui/registry/macros.hpp>

#include <QGuiApplication>
#include <QVector>

#include <filesystem>

fwGuiRegisterMacro( ::fwGuiQml::dialog::MessageDialog, ::sight::gui::dialog::IMessageDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

//value of the enum in QMessageBox
typedef const std::map< gui::dialog::IMessageDialog::Icons, std::string> MessageDialogQmlIconsType;
MessageDialogQmlIconsType messageDialogQmlIcons =
{
    {::gui::dialog::IMessageDialog::NONE, ""},
    {::gui::dialog::IMessageDialog::QUESTION, "fwGuiQml-" FWGUIQML_VER "/question.svg"},
    {::gui::dialog::IMessageDialog::INFO, "fwGuiQml-" FWGUIQML_VER "/information.svg"},
    {::gui::dialog::IMessageDialog::WARNING, "fwGuiQml-" FWGUIQML_VER "/warning.svg"},
    {::gui::dialog::IMessageDialog::CRITICAL, "fwGuiQml-" FWGUIQML_VER "/critical.svg"}
};

// value of the enum in int value of Dialog
typedef const std::map< gui::dialog::IMessageDialog::Buttons,
                        StandardButton::ButtonList> MessageDialogQmlButtonType;
MessageDialogQmlButtonType messageDialogQmlButton =
{
    {::gui::dialog::IMessageDialog::OK, StandardButton::ButtonList::Ok},
    {::gui::dialog::IMessageDialog::CANCEL, StandardButton::ButtonList::Cancel},
    {::gui::dialog::IMessageDialog::YES, StandardButton::ButtonList::Yes},
    {::gui::dialog::IMessageDialog::NO, StandardButton::ButtonList::No}
};

//------------------------------------------------------------------------------

MessageDialog::MessageDialog(gui::GuiBaseObject::Key key)
{
    qmlRegisterType<StandardButton>("Dialog", 1, 0, "StandardButton");
}

//------------------------------------------------------------------------------

MessageDialog::~MessageDialog()
{
}

//------------------------------------------------------------------------------

void MessageDialog::setTitle( const std::string& title )
{
    m_title = QString::fromStdString(title);
}

//------------------------------------------------------------------------------

void MessageDialog::setMessage( const std::string& msg )
{
    m_message = QString::fromStdString(msg);
}

//------------------------------------------------------------------------------

void MessageDialog::setIcon( gui::dialog::IMessageDialog::Icons icon )
{
    m_icon = icon;
}

//------------------------------------------------------------------------------

void MessageDialog::addButton( gui::dialog::IMessageDialog::Buttons button )
{
    m_buttons = (gui::dialog::IMessageDialog::Buttons) ( m_buttons | button );
}

//------------------------------------------------------------------------------

void MessageDialog::addCustomButton(const std::string& label, std::function<void()> clickedFn)
{
}

//-----------------------------------------------------------------------------

void MessageDialog::setDefaultButton(gui::dialog::IMessageDialog::Buttons button )
{
}

//------------------------------------------------------------------------------

::gui::dialog::IMessageDialog::Buttons MessageDialog::show()
{
    MessageDialogQmlIconsType::const_iterator iterIcon = messageDialogQmlIcons.find(m_icon);
    SLM_ASSERT("Unknown Icon", iterIcon != messageDialogQmlIcons.end());
    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    std::string icon = iterIcon->second;

    // get the path of the qml ui file in the 'rc' directory
    const auto& dialogPath = core::runtime::getLibraryResourceFilePath(
        "fwGuiQml-" FWGUIQML_VER "/dialog/MessageDialog.qml");
    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("messageDialog", this);
    // load the qml ui component
    QObject* window = engine->createComponent(dialogPath, context);
    SLM_ASSERT("The Qml File MessageDialog is not found or not loaded", window);
    // keep window to destroy it

    window->setProperty("title", m_title);
    QObject* dialog = window->findChild<QObject*>("dialog");
    SLM_ASSERT("The dialog is not found inside the window", dialog);
    StandardButton* buttonSetting = qobject_cast<StandardButton*>(dialog->findChild<QObject*>("standardButton"));
    Q_EMIT messageChanged();
    //set icon
    auto pathIcon = core::runtime::getLibraryResourceFilePath(icon);
    if (!std::filesystem::exists(pathIcon))
    {
        pathIcon = "";
    }
    emitIcon(QUrl::fromLocalFile(QString::fromStdString(pathIcon.string())));
    emitButtons(buttonSetting);

    m_clicked = gui::dialog::IMessageDialog::CANCEL;
    QEventLoop loop;
    //slot to retrieve the result and open the dialog with invoke
    connect(dialog, SIGNAL(accepted()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(rejected()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(reset()), &loop, SLOT(quit()));
    connect(window, SIGNAL(closing(QQuickCloseEvent*)), &loop, SLOT(quit()));
    QMetaObject::invokeMethod(dialog, "open");
    loop.exec();
    delete window;
    return m_clicked;
}

//------------------------------------------------------------------------------

void MessageDialog::resultDialog(int clicked)
{
    for(MessageDialogQmlButtonType::value_type button :  messageDialogQmlButton)
    {
        // get the button that has been click
        if ( clicked == button.second)
        {
            m_clicked = button.first;
            break;
        }
    }
}

//------------------------------------------------------------------------------

void MessageDialog::emitIcon(const QUrl& iconPath)
{
    m_iconImage = iconPath;
    Q_EMIT iconChanged();
}

//------------------------------------------------------------------------------

void MessageDialog::emitButtons(StandardButton* standardButton)
{
    int buttons = 0;
    // add the different type of button needed
    for(MessageDialogQmlButtonType::value_type button :  messageDialogQmlButton)
    {
        if ( m_buttons & button.first)
        {
            buttons |= button.second;
        }
    }
    standardButton->setButton(static_cast<StandardButton::ButtonList>(buttons));
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml

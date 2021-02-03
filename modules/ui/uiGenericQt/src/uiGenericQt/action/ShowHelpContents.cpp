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

#include "uiGenericQt/action/ShowHelpContents.hpp"

#include <core/base.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QHelpContentWidget>
#include <QHelpEngine>
#include <QObject>
#include <QSplitter>
#include <QTextBrowser>

#include <filesystem>

namespace uiGenericQt
{
namespace action
{

//------------------------------------------------------------------------------
/** @brief Help browser */
class HelpBrowser : public QTextBrowser
{
public:
    HelpBrowser(QHelpEngine* helpEngine, QWidget* parent = 0) :
        QTextBrowser(parent),
        helpEngine(helpEngine)
    {
    }

    //------------------------------------------------------------------------------

    QVariant loadResource(int type, const QUrl& url)
    {
        if (url.scheme() == "qthelp")
        {
            return QVariant(helpEngine->fileData(url));
        }
        else
        {
            return QTextBrowser::loadResource(type, url);
        }
    }

private:
    QHelpEngine* helpEngine;
};
//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiGenericQt::action::ShowHelpContents, data::Object )

//------------------------------------------------------------------------------

ShowHelpContents::ShowHelpContents( ) noexcept :
    m_bServiceIsConfigured(false),
    m_fsHelpPath("")
{
}

//------------------------------------------------------------------------------

ShowHelpContents::~ShowHelpContents() noexcept
{
}

//------------------------------------------------------------------------------

void ShowHelpContents::info(std::ostream& _sstream )
{
    _sstream << "Action for show help contents" << std::endl;
}

//------------------------------------------------------------------------------

void ShowHelpContents::configuring()
{
    /*
     * .qhp/.qch (source/binary) : Contains a table of contents,
     *                             an index of items in the documentation, and a file manifest.
     * .qhcp/.qhc (source/binary): Contains information that is used to customize
     *                             the appearance and available features of Qt Assistant.
     */
    this->::fwGui::IActionSrv::initialize();
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getExistingAttributeValue("id");
        m_fsHelpPath           = std::filesystem::path( filename );
        m_bServiceIsConfigured = std::filesystem::exists(m_fsHelpPath);
        SLM_WARN_IF("Help file " <<filename<< " doesn't exist", !m_bServiceIsConfigured);
    }
}

//------------------------------------------------------------------------------

void ShowHelpContents::updating()
{
    SLM_ASSERT("The Help service isn't configured properly.", m_bServiceIsConfigured);

    QDialog* dialog = new QDialog(qApp->activeWindow());
    dialog->setWindowTitle(QString("Help"));
    QHelpEngine* helpEngine = new QHelpEngine(QString::fromStdString(m_fsHelpPath.string()), dialog);
    if (!helpEngine->setupData())
    {
        SLM_ERROR("HelpEngine error: " << helpEngine->error().toStdString());
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( "Help file is missing or not correct." );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
        // Setup help engine information failed.
        // qhc (Qt Help Collection) or qch (Qt Compressed Help) file is not correct.
    }
    else
    {
        QSplitter* helpPanel     = new QSplitter(Qt::Horizontal);
        HelpBrowser* helpBrowser = new HelpBrowser(helpEngine, dialog);
        helpPanel->insertWidget(0, helpEngine->contentWidget());
        helpPanel->insertWidget(1, helpBrowser);
        helpPanel->setStretchFactor(1, 1);

        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->addWidget(helpPanel);
        dialog->setLayout( hLayout );
        QObject::connect(helpEngine->contentWidget(), SIGNAL(linkActivated(const QUrl&)), helpBrowser,
                         SLOT(setSource(const QUrl&)));

        dialog->exec();
    }
}

//------------------------------------------------------------------------------

void ShowHelpContents::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void ShowHelpContents::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiGenericQt

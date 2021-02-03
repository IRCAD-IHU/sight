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

#include "ioGdcm/SDicomSeriesDBReader.hpp"

#include <core/com/Signal.hxx>
#include <core/tools/System.hpp>

#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/LoggerDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwIO/IReader.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>

#include <string>
#include <vector>

namespace ioGdcm
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioGdcm::SDicomSeriesDBReader, ::fwMedData::SeriesDB )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesDBReader::SDicomSeriesDBReader() noexcept :
    m_cancelled(false),
    m_showLogDialog(true),
    m_dicomDirSupport(USER_SELECTION)
{
    m_sigJobCreated = newSignal<JobCreatedSignal>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

SDicomSeriesDBReader::~SDicomSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::configuring()
{
    ::fwIO::IReader::configuring();

    // Show log dialog
    core::runtime::ConfigurationElement::sptr logDialog = m_configuration->findConfigurationElement("showLogDialog");
    if(logDialog)
    {
        std::string logDialogStr = logDialog->getValue();
        SLM_ASSERT("<showLogDialog> value must be 'yes' or 'no'", logDialogStr == "yes" || logDialogStr == "no");
        m_showLogDialog = (logDialogStr == "yes");
    }

    // Enable dicomdir
    core::runtime::ConfigurationElement::sptr dicomDir = m_configuration->findConfigurationElement("dicomdirSupport");
    if(dicomDir)
    {
        std::string dicomDirStr = dicomDir->getValue();
        SLM_ASSERT("<dicomdirSupport> value must be 'always' or 'never' or 'user_selection'",
                   dicomDirStr == "always" || dicomDirStr == "never" || dicomDirStr == "user_selection");

        if(dicomDirStr == "always")
        {
            m_dicomDirSupport = ALWAYS;
        }
        else if(dicomDirStr == "never")
        {
            m_dicomDirSupport = NEVER;
        }
        else if(dicomDirStr == "user_selection")
        {
            m_dicomDirSupport = USER_SELECTION;
        }
    }
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? this->getSelectorDialogTitle() : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setType(::fwGui::dialog::LocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder( result->getFolder() );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::starting()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::stopping()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::info(std::ostream& _sstream )
{
    _sstream << "SDicomSeriesDBReader::info";
}

//------------------------------------------------------------------------------

std::string SDicomSeriesDBReader::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

::fwMedData::SeriesDB::sptr SDicomSeriesDBReader::createSeriesDB(const std::filesystem::path& dicomDir)
{
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    ::fwMedData::SeriesDB::sptr seriesDB      = ::fwMedData::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(dicomDir);

    auto job = reader->getJob();
    m_sigJobCreated->emit(job);

    if(m_dicomDirSupport == USER_SELECTION && reader->isDicomDirAvailable())
    {
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Dicomdir file");
        messageBox.setMessage( "There is a dicomdir file in the root folder. "
                               "Would you like to use it for the reading process ?" );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::YES_NO);
        ::fwGui::dialog::IMessageDialog::Buttons button = messageBox.show();

        reader->setDicomdirActivated(button == ::fwGui::dialog::IMessageDialog::YES);
    }
    else if(m_dicomDirSupport == ALWAYS)
    {
        reader->setDicomdirActivated(true);
    }
    else //m_dicomDirSupport == NEVER
    {
        reader->setDicomdirActivated(false);
    }

    try
    {
        reader->readDicomSeries();

        // Retrieve logger
        ::fwLog::Logger::sptr logger = reader->getLogger();
        logger->sort();

        // Display logger dialog if enabled
        if(m_showLogDialog && !logger->empty())
        {
            std::stringstream ss;
            if(seriesDB->size() > 1)
            {
                ss << "The reading process is over : <b>" << seriesDB->size() << " series</b> have been found. "
                    "<br>Please verify the log report to be informed of the potential errors.";
            }
            else
            {
                ss << "The reading process is over : <b>" << seriesDB->size() << " series</b> has been found. "
                    "<br>Please verify the log report to be informed of the potential errors.";
            }

            bool result = false;
            if(!reader->getJob()->cancelRequested())
            {
                result = ::fwGui::dialog::LoggerDialog::showLoggerDialog("Reading process over", ss.str(), logger);
            }

            // If the user cancel the reading process we delete the loaded series
            if(!result || reader->getJob()->cancelRequested())
            {
                ::fwMedDataTools::helper::SeriesDB sDBhelper(seriesDB);
                sDBhelper.clear();
            }
        }
    }
    catch (const std::exception& e)
    {
        m_readFailed = true;
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        ::fwGui::dialog::MessageDialog::show(
            "Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        m_readFailed = true;
        ::fwGui::dialog::MessageDialog::show(
            "Warning", "Warning during loading", ::fwGui::dialog::IMessageDialog::WARNING);
    }

    m_cancelled = job->cancelRequested();

    return seriesDB;
}

//------------------------------------------------------------------------------

void SDicomSeriesDBReader::updating()
{
    if( this->hasLocationDefined() )
    {
        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        ::fwMedData::SeriesDB::sptr seriesDB = this->createSeriesDB(this->getFolder() );

        if( seriesDB->size() > 0 && !m_cancelled)
        {
            // Retrieve dataStruct associated with this service
            ::fwMedData::SeriesDB::sptr associatedSeriesDB =
                this->getInOut< ::fwMedData::SeriesDB >(::fwIO::s_DATA_KEY);
            SLM_ASSERT("associated SeriesDB not instanced", associatedSeriesDB);

            // Clear SeriesDB and add new series
            ::fwMedDataTools::helper::SeriesDB sDBhelper(associatedSeriesDB);
            ::fwData::mt::ObjectWriteLock lock(associatedSeriesDB);
            sDBhelper.clear();
            // Notify removal.
            sDBhelper.notify();
            {
                ::fwData::mt::ObjectWriteLock lock(seriesDB);
                associatedSeriesDB->shallowCopy(seriesDB);
            }

            ::fwMedData::SeriesDB::ContainerType addedSeries = associatedSeriesDB->getContainer();

            auto sig = associatedSeriesDB->signal< ::fwMedData::SeriesDB::AddedSeriesSignalType >(
                ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG);
            sig->asyncEmit(addedSeries);
        }
        cursor.setDefaultCursor();
    }
    else
    {
        m_readFailed = true;
    }
}

//-----------------------------------------------------------------------------

::fwIO::IOPathType SDicomSeriesDBReader::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace ioGdcm

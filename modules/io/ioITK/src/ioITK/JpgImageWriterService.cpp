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

#include "ioITK/JpgImageWriterService.hpp"

#include <core/base.hpp>

#include <data/Image.hpp>
#include <data/location/Folder.hpp>

#include <fwIO/IWriter.hpp>

#include <fwItkIO/JpgImageWriter.hpp>

#include <gui/Cursor.hpp>
#include <gui/dialog/LocationDialog.hpp>
#include <gui/dialog/MessageDialog.hpp>
#include <gui/dialog/ProgressDialog.hpp>

#include <services/macros.hpp>

namespace ioITK
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioITK::JpgImageWriterService, ::sight::data::Image )

//------------------------------------------------------------------------------

JpgImageWriterService::JpgImageWriterService() noexcept
{
}

//------------------------------------------------------------------------------

JpgImageWriterService::~JpgImageWriterService() noexcept
{
}

//------------------------------------------------------------------------------

::fwIO::IOPathType JpgImageWriterService::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

void JpgImageWriterService::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void JpgImageWriterService::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void JpgImageWriterService::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    gui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a directory to save image" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(gui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(gui::dialog::ILocationDialog::FOLDER);

    data::location::Folder::sptr result;
    result = data::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder(result->getFolder());
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void JpgImageWriterService::starting()
{
}

//------------------------------------------------------------------------------

void JpgImageWriterService::stopping()
{
}

//------------------------------------------------------------------------------

void JpgImageWriterService::info(std::ostream& _sstream )
{
    _sstream << "JpgImageWriterService::info";
}

//------------------------------------------------------------------------------

void JpgImageWriterService::saveImage(const std::filesystem::path& imgPath, const CSPTR(data::Image)& img)
{
    ::fwItkIO::JpgImageWriter::sptr writer = ::fwItkIO::JpgImageWriter::New();
    gui::dialog::ProgressDialog progressMeterGUI("Saving image... ");

    data::location::Folder::sptr loc = data::location::Folder::New();
    loc->setFolder(imgPath);
    writer->setLocation(loc);
    writer->setObject(img);

    try
    {
        writer->addHandler( progressMeterGUI );
        writer->write();

    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        gui::dialog::MessageDialog::show("Warning",
                                         ss.str(),
                                         gui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        gui::dialog::MessageDialog::show("Warning",
                                         "Warning during saving",
                                         gui::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

void JpgImageWriterService::updating()
{

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        data::Image::csptr image = this->getInput< data::Image >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The input key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", image);

        gui::Cursor cursor;
        cursor.setCursor(gui::ICursor::BUSY);
        saveImage(this->getFolder(), image);
        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

} // namespace ioITK

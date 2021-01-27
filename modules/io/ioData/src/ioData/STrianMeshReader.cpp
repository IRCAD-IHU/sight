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

#include "ioData/STrianMeshReader.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>
#include <data/Mesh.hpp>

#include <fwDataIO/reader/MeshReader.hpp>

#include <fwIO/IReader.hpp>

#include <gui/dialog/LocationDialog.hpp>
#include <gui/dialog/MessageDialog.hpp>

#include <services/macros.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

fwServicesRegisterMacro( ::fwIO::IReader, ::ioData::STrianMeshReader, ::sight::data::Mesh )

namespace ioData
{

//-----------------------------------------------------------------------------

void STrianMeshReader::info(std::ostream& _sstream )
{
    this->SuperClass::info( _sstream );
    _sstream << std::endl << "Trian file reader";
}

//-----------------------------------------------------------------------------

std::vector< std::string > STrianMeshReader::getSupportedExtensions()
{
    std::vector< std::string > extensions;
    extensions.push_back(".trian");
    return extensions;
}

//------------------------------------------------------------------------------

::fwIO::IOPathType STrianMeshReader::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void STrianMeshReader::configuring()
{
    ::fwIO::IReader::configuring();
}

//------------------------------------------------------------------------------

void STrianMeshReader::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void STrianMeshReader::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    gui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a trian file" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Trian file", "*.trian");
    dialogFile.setOption(gui::dialog::ILocationDialog::READ);

    data::location::SingleFile::sptr result;
    result = data::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void STrianMeshReader::updating()
{
    if( this->hasLocationDefined() )
    {
        // Retrieve object
        data::Mesh::sptr mesh = this->getInOut< data::Mesh >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("mesh not instanced", mesh);

        ::fwDataIO::reader::MeshReader::sptr reader = ::fwDataIO::reader::MeshReader::New();
        reader->setObject( mesh );
        reader->setFile(this->getFile());

        try
        {
            // Launch reading process
            reader->read();
            // Notify reading
            data::Object::ModifiedSignalType::sptr sig;
            sig = mesh->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
            {
                core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }
        catch (const std::exception& e)
        {
            m_readFailed = true;
            std::stringstream ss;
            ss << "Warning during loading : " << e.what();

            gui::dialog::MessageDialog::show(
                "Warning",
                ss.str(),
                gui::dialog::IMessageDialog::WARNING);
        }
        catch( ... )
        {
            m_readFailed = true;
            gui::dialog::MessageDialog::show(
                "Warning",
                "Warning during loading.",
                gui::dialog::IMessageDialog::WARNING);
        }
    }
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

}

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioData/STrianMeshReader.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/Mesh.hpp>

#include <fwDataIO/reader/MeshReader.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwIO/IReader.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem/operations.hpp>

#include <fstream>
#include <iostream>

fwServicesRegisterMacro( ::fwIO::IReader, ::ioData::STrianMeshReader, ::fwData::Mesh );

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
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a trian file" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Trian file", "*.trian");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
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
    SLM_TRACE_FUNC();
    if( this->hasLocationDefined() )
    {
        // Retrieve object
        ::fwData::Mesh::sptr mesh = this->getInOut< ::fwData::Mesh >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("mesh not instanced", mesh);

        ::fwDataIO::reader::MeshReader::sptr reader = ::fwDataIO::reader::MeshReader::New();
        reader->setObject( mesh );
        reader->setFile(this->getFile());

        try
        {
            // Launch reading process
            reader->read();
            // Notify reading
            ::fwData::Object::ModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }
        catch (const std::exception& e)
        {
            std::stringstream ss;
            ss << "Warning during loading : " << e.what();

            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning",
                ss.str(),
                ::fwGui::dialog::IMessageDialog::WARNING);
        }
        catch( ... )
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning",
                "Warning during loading.",
                ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
}

}

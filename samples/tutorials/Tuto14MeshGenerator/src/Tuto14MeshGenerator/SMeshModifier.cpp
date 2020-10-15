/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "Tuto14MeshGenerator/SMeshModifier.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Mesh.hpp>

#include <fwDataTools/Mesh.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

namespace Tuto14MeshGenerator
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::Tuto14MeshGenerator::SMeshModifier, ::fwData::Mesh )

//-----------------------------------------------------------------------------

SMeshModifier::SMeshModifier() noexcept
{
}

//-----------------------------------------------------------------------------

SMeshModifier::~SMeshModifier() noexcept
{
}

//-----------------------------------------------------------------------------

void SMeshModifier::configuring()
{
    this->initialize();

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");

    SLM_ASSERT("There must be one (and only one) <config functor=... /> element.", vectConfig.size() == 1 );
    ::fwRuntime::ConfigurationElement::sptr configElement = vectConfig.at(0);

    SLM_ASSERT( "There must be a functor attribute in the <config> xml element.",
                configElement->hasAttribute("functor") );
    m_functor = configElement->getExistingAttributeValue("functor");
    SLM_ASSERT("Wrong functor name "<<m_functor << " (required GenTriangle, GenQuad or GenTriangleQuad)",
               m_functor == "ShakeMeshPoint"
               || m_functor == "ColorizeMeshPoints"
               || m_functor == "ColorizeMeshCells"
               || m_functor == "ComputePointNormals"
               || m_functor == "ComputeCellNormals"
               || m_functor == "ShakePointNormals"
               || m_functor == "ShakeCellNormals"
               || m_functor == "MeshDeformation"
               );

}

//-----------------------------------------------------------------------------

void SMeshModifier::starting()
{
    this->actionServiceStarting();
    ::fwDataTools::Mesh::initRand();
}

//-----------------------------------------------------------------------------

void SMeshModifier::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void SMeshModifier::updating()
{
    ::fwData::Mesh::sptr mesh = this->getInOut< ::fwData::Mesh >("mesh");
    SLM_ASSERT("Mesh dynamicCast failed", mesh);
    try
    {
        if(m_functor == "ShakeMeshPoint")
        {
            ::fwDataTools::Mesh::shakePoint(mesh);

            ::fwData::Mesh::VertexModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::VertexModifiedSignalType >(::fwData::Mesh::s_VERTEX_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ColorizeMeshCells")
        {
            ::fwDataTools::Mesh::colorizeMeshCells(mesh);

            ::fwData::Mesh::CellColorsModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::CellColorsModifiedSignalType >(
                ::fwData::Mesh::s_CELL_COLORS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ColorizeMeshPoints")
        {
            ::fwDataTools::Mesh::colorizeMeshPoints(mesh);

            ::fwData::Mesh::PointColorsModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::PointColorsModifiedSignalType >(
                ::fwData::Mesh::s_POINT_COLORS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ComputeCellNormals")
        {
            ::fwDataTools::Mesh::generateCellNormals(mesh);

            ::fwData::Mesh::CellNormalsModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::CellNormalsModifiedSignalType >(
                ::fwData::Mesh::s_CELL_NORMALS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ComputePointNormals")
        {
            ::fwDataTools::Mesh::generatePointNormals(mesh);

            ::fwData::Mesh::PointNormalsModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::PointNormalsModifiedSignalType >(
                ::fwData::Mesh::s_POINT_NORMALS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ShakeCellNormals")
        {
            ::fwDataTools::Mesh::shakeCellNormals(mesh);

            ::fwData::Mesh::CellNormalsModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::CellNormalsModifiedSignalType >(
                ::fwData::Mesh::s_CELL_NORMALS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ShakePointNormals")
        {
            ::fwDataTools::Mesh::shakePointNormals(mesh);

            ::fwData::Mesh::PointNormalsModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::PointNormalsModifiedSignalType >(
                ::fwData::Mesh::s_POINT_NORMALS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "MeshDeformation")
        {
            m_animator.computeDeformation( mesh, 100, 50 );
            ::fwData::Object::ModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during generating : " << e.what();

        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning",
            ss.str(),
            ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//-----------------------------------------------------------------------------

void SMeshModifier::info(std::ostream& _sstream )
{
    _sstream << "MeshGenerator Action" << std::endl;
}

//-----------------------------------------------------------------------------

}

/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "fwMDSemanticPatch/V1/V2/fwData/Patient.hpp"

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwTools/UUID.hpp>

namespace fwMDSemanticPatch
{
namespace V1
{
namespace V2
{
namespace fwData
{

Patient::Patient() :
    ::fwAtomsPatch::ISemanticPatch()
{
    m_originClassname = "::fwData::Patient";
    m_originVersion   = "1";
    this->addContext("MedicalData", "V1", "V2");
}

// ----------------------------------------------------------------------------

Patient::~Patient()
{
}

// ----------------------------------------------------------------------------

Patient::Patient( const Patient& cpy ) :
    ::fwAtomsPatch::ISemanticPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void Patient::apply(
    const ::fwAtoms::Object::sptr& previous,
    const ::fwAtoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    ISemanticPatch::apply(previous, current, newVersions);
    ::fwAtomsPatch::helper::cleanFields( current );
    ::fwAtomsPatch::helper::Object helper( current );

    ::fwAtoms::String::sptr uid = current->getAttribute< ::fwAtoms::String >("patient_id");
    if( uid->getValue().empty() )
    {
        helper.replaceAttribute("patient_uid", ::fwAtoms::String::New(::fwTools::UUID::generateUUID()));
    }
}

// ----------------------------------------------------------------------------

} // namespace fwData
} // namespace V2
} // namespace V1
} // namespace fwMDSemanticPatch

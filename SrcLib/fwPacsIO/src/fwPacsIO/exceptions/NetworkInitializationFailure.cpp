/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwPacsIO/exceptions/NetworkInitializationFailure.hpp"

namespace fwPacsIO
{
namespace exceptions
{

NetworkInitializationFailure::NetworkInitializationFailure(const std::string& message) :
    ::fwPacsIO::exceptions::Base(message)
{
}

// ----------------------------------------------------------------------------

NetworkInitializationFailure::~NetworkInitializationFailure() throw()
{
}

} //exceptions
} //fwPacsIO

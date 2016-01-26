/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWPACSIO_EXCEPTIONS_NETWORKINITIALIZATIONFAILURE_HPP__
#define __FWPACSIO_EXCEPTIONS_NETWORKINITIALIZATIONFAILURE_HPP__

#include "fwPacsIO/config.hpp"
#include "fwPacsIO/exceptions/Base.hpp"

namespace fwPacsIO
{
namespace exceptions
{

/**
 * @class NetworkInitializationFailure
 * @brief NetworkInitializationFailure Exceptions
 * @date  2013.
 */
class FWPACSIO_CLASS_API NetworkInitializationFailure : public fwPacsIO::exceptions::Base
{
public:
    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    FWPACSIO_API NetworkInitializationFailure(const std::string& message);

    /// Destructor
    FWPACSIO_API virtual ~NetworkInitializationFailure() throw ();

};

} //exceptions
} //fwPacsIO


#endif /* __FWPACSIO_EXCEPTIONS_NETWORKINITIALIZATIONFAILURE_HPP__ */

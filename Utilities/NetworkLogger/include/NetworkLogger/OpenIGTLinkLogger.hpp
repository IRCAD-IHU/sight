/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __NETWORKLOGGER_OPENIGTLINKLOGGER_HPP__
#define __NETWORKLOGGER_OPENIGTLINKLOGGER_HPP__

#include "NetworkLogger/INetworkLogger.hpp"
#include "NetworkLogger/types.hpp"

#include <igtlNetwork/Client.hpp>

#include <boost/type.hpp>

namespace networkLogger
{
/**
 *
 * @brief OpenIGTLink logger version use OpenIGTLinkNetwork socket to receive/connect
 */
class OpenIGTLinkLogger : public INetworkLogger
{
public:

    /**
     * @typedef SocketType
     *
     * @brief define the OpenIGTLink::Client socket to be the socket type to use
     */
    typedef igtlNetwork::Client SocketType;

    /**
     * @typedef FunctionType
     *
     * @brief logger function type to use
     */
    typedef LoggerFunction<SocketType>::FunctionType FunctionType;

    ///Constructor
    OpenIGTLinkLogger();

    ///Destructor
    ~OpenIGTLinkLogger();

    /**
     * @brief log receive object from network to outputStream
     * @throw std::runtime_error
     */
    void log(std::ostream& outputStream);

    /**
     * @brief configure OpenIGTLinkLogger(hostname + port)
     *
     * @param[in] argc number of arguments
     * @param[in] argv arguments
     * @throw std::runtime_error
     */
    void configure(int argc, char** argv);

    /**
     * @brief create a OpenIGTLinkLogger with a user-defined function
     *
     * @param[in] logFunction log is like a incomplete method and the user-defined function complete the log method
     * @return smart pointer contain a instance of OpenIGTLinkLogger
     */
    static INetworkLogger::sptr create(FunctionType const& logFunction);
private:

    /// client socket
    ::igtlNetwork::Client m_client;

    /// port
    std::uint16_t m_port;

    /// host
    std::string m_host;

    /// user-defined function
    FunctionType m_logFunction;
};
}

#endif // __NETWORKLOGGER_OPENIGTLINKLOGGER_HPP__

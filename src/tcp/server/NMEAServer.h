/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
 A detailed list of copyright holders can be found in the file "AUTHORS".

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License version 3
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 }
 */

#ifndef SRC_TCP_SERVER_NMEASERVER_H_
#define SRC_TCP_SERVER_NMEASERVER_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <cstdint>
#include <boost/thread/mutex.hpp>
#include <string>
#include <vector>

#include "Connection.h"
#include "../../config/Parameters.h"

#define S_MAX_CLIENTS SERVER_MAX_CLIENTS

class NMEAServer
{
public:
    NMEAServer(const NMEAServer&) = delete;
    NMEAServer& operator=(const NMEAServer&) = delete;

    NMEAServer(boost::asio::signal_set& /*sigset*/, std::uint16_t /*port*/);
    virtual ~NMEAServer() noexcept;

    /**
     * Run server.
     */
    void run();

    /**
     * Write message to all clients.
     */
    void writeToAll(const std::string& /*msg*/) noexcept;

private:
    /**
     * Accept connections
     */
    void accept() noexcept;
    /**
     * Register stop-handler to signals
     */
    void awaitStop();
    /**
     * Stop all connections.
     */
    void stopAll();

    /**
     * Accept - handler
     */
    void handleAccept(const boost::system::error_code& /*ec*/) noexcept;

    /**
     * Mutex - threadsafety
     */
    boost::mutex mMutex;

    /**
     * Internal IO-service
     */
    boost::asio::io_service mIOservice;
    /**
     * Signals reference
     */
    boost::asio::signal_set& mrSigSet;
    /**
     * Acceptor
     */
    boost::asio::ip::tcp::acceptor mAcceptor;
    /**
     * Socket
     */
    boost::asio::ip::tcp::socket mSocket;
    /**
     * Container for connections.
     */
    std::vector<boost::shared_ptr<Connection>> mClients;
};

#endif /* SRC_TCP_SERVER_NMEASERVER_H_ */

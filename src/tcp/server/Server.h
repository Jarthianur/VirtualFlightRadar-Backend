/*
 Copyright_License {

 Copyright (C) 2016 VirtualFlightRadar-Backend
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

#ifndef SRC_TCP_SERVER_SERVER_H_
#define SRC_TCP_SERVER_SERVER_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <cstdint>
#include <boost/thread/mutex.hpp>
#include <string>
#include <vector>

#include "Connection.h"
#include "../../config/Parameters.h"

namespace tcp
{
namespace server
{

#define S_MAX_CLIENTS SERVER_MAX_CLIENTS

/**
 * The Server class.
 *
 * This class provides all functionality to run a standalone server.
 * A message may only be sent to all endpoints.
 */
class Server
{
public:
    /**
     * Non-copyable
     */
    Server(const Server&) = delete;
    /**
     * Not assignable
     */
    Server& operator=(const Server&) = delete;
    /**
     * Construct a Server with its port and
     * the signal set to handle interrupts.
     *
     * @param r_sigset the signal set
     * @param port the port where to open the server
     */
    Server(boost::asio::signal_set& /*r_sigset*/, std::uint16_t /*port*/);
    virtual ~Server() noexcept;
    /**
     * Run server.
     * This function returns as soon as every operation in the queue
     * has returned, what may only happen if the server got an interrupt.
     */
    void run();
    /**
     * Write a message to all clients.
     *
     * @param cr_msg the msg to write
     */
    void writeToAll(const std::string& /*cr_msg*/) noexcept;

private:
    /**
     * Accept Connections.
     *
     * @exceptsafe strong
     */
    void accept() noexcept;
    /**
     * Register stop-handler to signals
     */
    void awaitStop();
    /**
     * Stop all Connections.
     */
    void stopAll();
    /**
     * Accept - handler
     *
     * @param cr_ec the error code
     *
     * @exceptsafe strong
     */
    void handleAccept(const boost::system::error_code& /*cr_ec*/) noexcept;

    /// Mutex
    boost::mutex mMutex;
    /// Internal IO-service
    boost::asio::io_service mIOservice;
    /// Ref to signal set
    boost::asio::signal_set& mrSigSet;
    /// Acceptor
    boost::asio::ip::tcp::acceptor mAcceptor;
    /// Socket
    boost::asio::ip::tcp::socket mSocket;
    /// Vector holding Connections
    std::vector<boost::shared_ptr<Connection>> mClients;
};

}  // namespace server
}  // namespace tcp

#endif /* SRC_TCP_SERVER_SERVER_H_ */

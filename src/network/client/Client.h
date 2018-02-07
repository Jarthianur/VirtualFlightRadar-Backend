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

#ifndef SRC_NETWORK_CLIENT_CLIENT_H_
#define SRC_NETWORK_CLIENT_CLIENT_H_

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <string>
#include "../../config/Parameters.h"

namespace feed
{
class Feed;
}

namespace network
{
namespace client
{

/// Wait for (re-)connect timeout
#ifdef CLIENT_CONNECT_WAIT_TIMEVAL
#define C_CON_WAIT_TIMEVAL CLIENT_CONNECT_WAIT_TIMEVAL
#else
#define C_CON_WAIT_TIMEVAL 120
#endif

/**
 * @class Client
 * @brief Incomplete base-class representing a TCP client.
 * @note A Client is unique and only movable.
 */
class Client
{
public:
    /**
     * Non-copyable
     */
    Client(const Client&) = delete;
    /**
     * Not assignable
     */
    Client& operator=(const Client&) = delete;
    /**
     * @fn ~Client
     * @brief Destructor
     */
    virtual ~Client() noexcept;
    /**
     * @fn run
     * @brief Run the Client.
     * @note Returns after all queued handlers have returned.
     * @param r_sigset The signal set reference to register handler
     */
    void run(boost::asio::signal_set& r_sigset);
    /**
     * @fn stop
     * @brief Stop the Client and close the connection.
     */
    virtual void stop();

protected:
    /**
     * @fn Client
     * @brief Constructor
     * @param cr_host  The hostname
     * @param cr_port  The port
     * @param cr_comp  The component name
     * @param r_feed   The handler Feed reference
     */
    Client(const std::string& cr_host, const std::string& cr_port,
            const std::string& cr_comp, feed::Feed& r_feed);
    /**
     * @fn timedConnect
     * @brief Connect after timeout.
     */
    void timedConnect();
    /**
     * @fn read
     * @brief Read from remote endpoint.
     */
    virtual void read();
    /**
     * @fn connect
     * @brief Connect to host.
     * @note To be implemented.
     */
    virtual void connect() = 0;
    /**
     * @fn handleTimedConnect
     * @brief Handler for timedConnect.
     * @param cr_ec The error code
     */
    void handleTimedConnect(const boost::system::error_code& cr_ec) noexcept;
    /**
     * @fn handleRead
     * @brief Handler for read.
     * @param cr_ec The error code
     * @param s     The sent bytes
     */
    void handleRead(const boost::system::error_code& cr_ec, std::size_t s) noexcept;
    /**
     * @fn handleResolve
     * @brief Handler for resolve host.
     * @note Internally used in connect. To be implemented.
     * @param cr_ec The error code
     * @param it    The resolve iterator
     */
    virtual void handleResolve(const boost::system::error_code& cr_ec,
            boost::asio::ip::tcp::resolver::iterator it) noexcept = 0;
    /**
     * @fn handleConnect
     * @brief Handler for connect.
     * @note To be implemented.
     * @param cr_ec The error code
     * @param it    The resolve iterator
     */
    virtual void handleConnect(const boost::system::error_code& cr_ec,
            boost::asio::ip::tcp::resolver::iterator it) noexcept = 0;

    /// Internal IO-service
    boost::asio::io_service mIoService;
    /// Socket
    boost::asio::ip::tcp::socket mSocket;
    /// Resolver
    boost::asio::ip::tcp::resolver mResolver;
    /// Response string
    std::string mResponse;
    /// Read buffer
    boost::asio::streambuf mBuffer;
    /// Hostname
    const std::string mHost;
    /// Port
    const std::string mPort;
    /// Component string used for logging
    const std::string mComponent;
    /// Handler Feed reference
    feed::Feed& mrFeed;

private:
    /// Connection timer
    boost::asio::deadline_timer mConnectTimer;
};

}  // namespace client
}  // namespace network

#endif /* SRC_NETWORK_CLIENT_CLIENT_H_ */

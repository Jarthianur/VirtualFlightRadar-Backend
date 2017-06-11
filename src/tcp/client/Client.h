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

#ifndef SRC_TCP_CLIENT_CLIENT_H_
#define SRC_TCP_CLIENT_CLIENT_H_

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <string>
#include "../../config/Parameters.h"

namespace feed
{
class Feed;
}

namespace tcp
{
namespace client
{

/// Wait for (re-)connect timeout
#define C_CON_WAIT_TIMEVAL CLIENT_CONNECT_WAIT_TIMEVAL

/**
 * The Client base class.
 *
 * This class provides functionality to connect to a server
 * via TCP. Concrete Clients need to implement following methods:
 * - process
 * - connect
 * - handleResolve
 * - handleConnect
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
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~Client() noexcept;
    /**
     * Run the Client.
     * This function returns after all queued handles have returned.
     */
    void run(boost::asio::signal_set& /*r_sigset*/);

protected:
    /**
     * Construct a Client given a signal set, handling interrupts,
     * host, port, a string representing the Client type
     * and the Feed handling this Client.
     *
     * @param r_sigset the signal set
     * @param cr_host  the hostname
     * @param cr_port  the port
     * @param cr_comp  the component string
     * @param r_feed   the handler Feed
     */
    Client(const std::string& /*cr_host*/, const std::string& /*cr_port*/,
           const std::string& /*cr_comp*/, feed::Feed& /*r_feed*/);
    /**
     * Register stop-handler to signals.
     */
    void awaitStop();
    /**
     * Connect with timeout.
     */
    void timedConnect() noexcept;
    /**
     * Stop the Client and close the connection.
     */
    virtual void stop() noexcept;
    /**
     * Read data.
     */
    virtual void read() noexcept;
    /**
     * Process read data.
     */
    virtual void process() noexcept = 0;
    /**
     * Connect to host.
     */
    virtual void connect() noexcept = 0;
    /**
     * Timed connect - handler
     *
     * @param cr_ec the error code
     *
     * @exceptsafe strong
     */
    void handleTimedConnect(const boost::system::error_code& /*cr_ec*/)
            noexcept;
    /**
     * Read - handler
     *
     * @param cr_ec the error code
     * @param s     the sent bytes
     *
     * @exceptsafe strong
     */
    void handleRead(const boost::system::error_code& /*cr_ec*/,
                    std::size_t /*s*/) noexcept;
    /**
     * Resolve host - handler
     *
     * @param cr_ec the error code
     * @param it    the resolve iterator
     *
     * @exceptsafe strong
     */
    virtual void handleResolve(const boost::system::error_code& /*cr_ec*/,
                               boost::asio::ip::tcp::resolver::iterator /*it*/)
                                       noexcept = 0;
    /**
     * Connect - handler
     *
     * @param cr_ec the error code
     * @param it    the resolve iterator
     *
     * @exceptsafe strong
     */
    virtual void handleConnect(const boost::system::error_code& /*cr_ec*/,
                               boost::asio::ip::tcp::resolver::iterator /*it*/)
                                       noexcept = 0;

    /// Internal IO-service
    boost::asio::io_service mIOservice;
    /// Signal set reference
    boost::asio::signal_set& mrSigSet;
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
}  // namespace tcp

#endif /* SRC_TCP_CLIENT_CLIENT_H_ */

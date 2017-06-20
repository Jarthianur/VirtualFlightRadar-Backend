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

#ifndef SRC_TCP_CLIENT_GPSDCLIENT_H_
#define SRC_TCP_CLIENT_GPSDCLIENT_H_

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <string>

#include "Client.h"

namespace tcp
{
namespace client
{

/**
 * The GpsdClient class, extends and implements Client.
 *
 * This Client provides functionality to handle connections to
 * any GPSD server.
 */
class GpsdClient: public Client
{
public:
    /**
     * Non-copyable
     */
    GpsdClient(const GpsdClient&) = delete;
    /**
     * Not assignable
     */
    GpsdClient& operator=(const GpsdClient&) = delete;
    /**
     * Constructor
     *
     * @param r_sigset the signal set handling interrupts
     * @param cr_host  the hostname
     * @param cr_port  the port
     * @param cr_login the login string to transmit
     * @param r_feed   the handler Feed
     */
    GpsdClient(boost::asio::signal_set& r_sigset, const std::string& cr_host,
            const std::string& cr_port, vfrb::Feed& r_feed);
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~GpsdClient() noexcept;

private:
    /**
     * Implement Client::connect.
     *
     * @overload Client::connect
     */
    void connect() noexcept override;
    /**
     * Extend Client::stop
     * Send unwatch-request to server before disconnect.
     *
     * @overload Client::stop
     */
    void stop() noexcept override;
    /**
     * Implement Client::process
     *
     * @overload Client::process
     */
    void process() noexcept override;
    /**
     * Implement Client::handleResolve
     *
     * @overload Client::handleResolve
     */
    void handleResolve(const boost::system::error_code& cr_ec,
            boost::asio::ip::tcp::resolver::iterator it) noexcept override;
    /**
     * Implement Client::handleConnect
     *
     * @overload Client::handleConnect
     */
    void handleConnect(const boost::system::error_code& cr_ec,
            boost::asio::ip::tcp::resolver::iterator it) noexcept override;
    /**
     * Send watch-request - handler
     *
     * @param cr_ec the error code
     * @param s     the sent bytes
     *
     * @exceptsafe strong
     */
    void handleWatch(const boost::system::error_code& cr_ec, std::size_t s) noexcept;
};

}  // namespace client
}  // namespace tcp

#endif /* SRC_TCP_CLIENT_GPSDCLIENT_H_ */

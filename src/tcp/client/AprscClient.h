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

#ifndef SRC_TCP_CLIENT_APRSCCLIENT_H_
#define SRC_TCP_CLIENT_APRSCCLIENT_H_

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <string>

#include "Client.h"

namespace tcp
{
namespace client
{

/**
 * The AprscClient class, extends and implements Client.
 *
 * This Client provides functionality to handle connections to
 * any APRSC server.
 */
class AprscClient: public Client
{
public:
    /**
     * Non-copyable
     */
    AprscClient(const AprscClient&) = delete;
    /**
     * Not assignable
     */
    AprscClient& operator=(const AprscClient&) = delete;
    /**
     * Constructor
     *
     * @param cr_host  the hostname
     * @param cr_port  the port
     * @param cr_login the login string to transmit
     * @param r_feed   the handler Feed reference
     */
    AprscClient(const std::string& cr_host, const std::string& cr_port,
            const std::string& cr_login, feed::Feed& r_feed);
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~AprscClient() noexcept;

private:
    /**
     * Implement Client::connect.
     *
     * @overload Client::connect
     */
    void connect() noexcept override;
    /**
     * Extend Client::stop
     * Cancel timer before disconnect.
     *
     * @overload Client::stop
     */
    void stop() noexcept override;
    /**
     * Send a keep-alive beacon to APRSC every 10 minutes.
     */
    void sendKaBeacon() noexcept;
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
     * Send login string - handler
     *
     * @param cr_ec the error code
     * @param s     the sent bytes
     *
     * @exceptsafe strong
     */
    void handleLogin(const boost::system::error_code& cr_ec, std::size_t s) noexcept;
    /**
     * Send keep-alive beacon - handler
     *
     * @param cr_ec the error code
     * @param s     the sent bytes
     *
     * @exceptsafe strong
     */
    void handleSendKaBeacon(const boost::system::error_code& cr_ec, std::size_t s)
            noexcept;

    /// Login string
    std::string mLoginStr;
    /// Client stopped?
    bool mStopped;
    /// Beacon timer
    boost::asio::deadline_timer mTimeout;
};

}  // namespace client
}  // namespace tcp

#endif /* SRC_TCP_CLIENT_APRSCCLIENT_H_ */

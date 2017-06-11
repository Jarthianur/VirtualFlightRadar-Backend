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

#ifndef SRC_TCP_CLIENT_SENSORCLIENT_H_
#define SRC_TCP_CLIENT_SENSORCLIENT_H_

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <string>

#include "Client.h"
#include "../../config/Parameters.h"

namespace tcp
{
namespace client
{

#define WC_RCV_TIMEOUT WINDCLIENT_RECEIVE_TIMEOUT

/**
 * The SensorClient class, extends and implements Client.
 *
 * This Client provides functionality to handle connections to
 * any server sending sensor information through NMEA MDA,WMV sentences.
 */
class SensorClient: public Client
{
public:
    /**
     * Non-copyable
     */
    SensorClient(const SensorClient&) = delete;
    /**
     * Not assignable
     */
    SensorClient& operator=(const SensorClient&) = delete;
    /**
     * Constructor
     *
     * @param cr_host  the hostname
     * @param cr_port  the port
     * @param cr_login the login string to transmit
     * @param r_feed   the handler Feed reference
     */
    SensorClient(const std::string& /*cr_host*/, const std::string& /*cr_port*/,
                 feed::Feed& /*r_feed*/);
    /**
     * Destructor
     *
     * @exceptsafe no-throw
     */
    virtual ~SensorClient() noexcept;

private:
    /**
     * Extend Client::read
     * Read with timeout.
     *
     * @overload Client::read
     */
    void read() noexcept override;
    /**
     * Implement Client::connect.
     *
     * @overload Client::connect
     */
    void connect() noexcept override;
    /**
     * Implement Client::process
     *
     * @overload Client::process
     */
    void process() noexcept override;
    /**
     * Check read timeout deadline reached.
     *
     * @exceptsafe strong
     */
    void checkDeadline() noexcept;
    /**
     * Extend Client::stop
     * Cancel timer before disconnect.
     *
     * @overload Client::stop
     */
    void stop() noexcept override;
    /**
     * Implement Client::handleResolve
     *
     * @overload Client::handleResolve
     */
    void handleResolve(const boost::system::error_code& /*cr_ec*/,
                       boost::asio::ip::tcp::resolver::iterator /*it*/)
                               noexcept override;
    /**
     * Implement Client::handleConnect
     *
     * @overload Client::handleConnect
     */
    void handleConnect(const boost::system::error_code& /*cr_ec*/,
                       boost::asio::ip::tcp::resolver::iterator /*it*/)
                               noexcept override;

    /// Client stopped?
    bool mStopped;
    /// Read timer
    boost::asio::deadline_timer mTimeout;
};

}  // namespace client
}  // namespace tcp

#endif /* SRC_TCP_CLIENT_SENSORCLIENT_H_ */

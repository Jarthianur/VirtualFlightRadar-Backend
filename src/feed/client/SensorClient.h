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

#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "Client.h"

namespace feed
{
class Feed;
} /* namespace feed */

#ifdef WINDCLIENT_RECEIVE_TIMEOUT
#define WC_RCV_TIMEOUT WINDCLIENT_RECEIVE_TIMEOUT
#else
#define WC_RCV_TIMEOUT 5
#endif

namespace feed
{
namespace client
{
/**
 * @class SensorClient extends Client
 * @brief Handle connections to a server sending sensor information in NMEA MDA,WMV
 * sentences.
 * @see Client.h
 */
class SensorClient : public Client
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
     * @fn SensorClient
     * @brief Constructor
     * @param cr_host  The hostname
     * @param cr_port  The port
     * @param cr_login The login string to transmit
     * @param r_feed   The handler Feed reference
     */
    SensorClient(const std::string& crHost, const std::string& crPort, feed::Feed& rFeed);

    /**
     * @fn ~SensorClient
     * @brief Destructor
     */
    virtual ~SensorClient() noexcept;

private:
    /**
     * @fn read
     * @brief Read with timeout.
     * @override Client::read
     */
    void read() override;

    /**
     * @fn connect
     * @override Client::connect
     */
    void connect() override;

    /**
     * @fn checkDeadline
     * @brief Check read timeout deadline reached.
     */
    void checkDeadline();

    /**
     * @fn stop
     * @brief Cancel timer before stop.
     * @override Client::stop
     */
    void stop() override;

    /**
     * @fn handleResolve
     * @override Client::handleResolve
     */
    void
    handleResolve(const boost::system::error_code& crError,
                  boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept override;

    /**
     * @fn handleConnect
     * @override Client::handleConnect
     */
    void
    handleConnect(const boost::system::error_code& crError,
                  boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept override;

    /// Client stopped?
    bool mStopped;

    /// Read timer
    boost::asio::deadline_timer mTimeout;
};

}  // namespace client
}  // namespace network

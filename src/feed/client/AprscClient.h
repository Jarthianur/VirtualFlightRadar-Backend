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

#include <cstddef>
#include <string>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "Client.h"

/// @namespace feed
namespace feed
{
/// @namespace client
namespace client
{
/**
 * @class AprscClient extends Client
 * @brief Handle connections to APRSC server.
 * @see Client.h
 */
class AprscClient : public Client
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
     * @fn AprscClient
     * @brief Constructor
     * @param cr_host  The hostname
     * @param cr_port  The port
     * @param cr_login The login string to transmit
     * @param r_feed   The handler Feed reference
     */
    AprscClient(const std::string& crHost, const std::string& crPort,
                const std::string& crLogin, feed::Feed& rFeed);
    /**
     * @fn ~AprscClient
     * @brief Destructor
     */
    virtual ~AprscClient() noexcept;

private:
    /**
     * @fn connect
     * @override Client::connect
     */
    void connect() override;

    /**
     * @fn stop
     * @brief Cancel timer before stop.
     * @override Client::stop
     */
    void stop() override;

    /**
     * @fn sendKaBeacon
     * @brief Send a keep-alive beacon to APRSC every 10 minutes.
     */
    void sendKeepAliveBeacon();

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

    /**
     * @fn handleLogin
     * @brief Handler fro send login string.
     * @param cr_ec The error code
     * @param s     The sent bytes
     */
    void handleLogin(const boost::system::error_code& crError,
                     std::size_t vBytes) noexcept;

    /**
     * @fn handleSendKaBeacon
     * @brief Handler for sendKaBeacon
     * @param cr_ec The error code
     * @param s     The sent bytes
     */
    void handleSendKaBeacon(const boost::system::error_code& crError,
                            std::size_t vBytes) noexcept;

    /// Login string
    std::string mLoginStr;

    /// Client stopped?
    bool mStopped;

    /// Beacon timer
    boost::asio::deadline_timer mTimeout;
};

}  // namespace client
}  // namespace feed

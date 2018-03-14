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

namespace feed
{
namespace client
{
/**
 * @class GpsdClient extends Client.
 * @brief Handle connections to a GPSD server.
 * @see Client.h
 */
class GpsdClient : public Client
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
     * @fn GpsdClient
     * @brief Constructor
     * @param cr_host  The hostname
     * @param cr_port  The port
     * @param cr_login The login string to transmit
     * @param r_feed   The handler Feed reference
     */
    GpsdClient(const std::string& crHost, const std::string& crPort, feed::Feed& rFeed);

    /**
     * @fn ~GpsdClient
     * @brief Destructor
     */
    virtual ~GpsdClient() noexcept;

private:
    /**
     * @fn connect.
     * @override Client::connect
     */
    void connect() override;

    /**
     * @fn stop
     * @brief Send unwatch-request to server before stop.
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

    /**
     * @fn handleWatch
     * @brief Handler for send watch-reques
     * @param cr_ec The error code
     * @param s     The sent bytes
     */
    void handleWatch(const boost::system::error_code& crError,
                     std::size_t vBytes) noexcept;
};

}  // namespace client
}  // namespace network

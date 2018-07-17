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

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "../../Defines.h"
#include "Client.hpp"

#ifdef WINDCLIENT_RECEIVE_TIMEOUT
#define WC_RCV_TIMEOUT WINDCLIENT_RECEIVE_TIMEOUT
#else
#define WC_RCV_TIMEOUT 5
#endif

/// @namespace feed
namespace feed
{
/// @namespace client
namespace client
{
/**
 * @class SensorClient
 * @brief Connect to a sender of sensoric information.
 * @extends Client
 */
class SensorClient : public Client
{
public:
    NOT_COPYABLE(SensorClient)

    /**
     * @fn SensorClient
     * @brief Constructor
     * @param crHost The hostname
     * @param crPort The port
     * @param rFeed  The handler Feed reference
     */
    explicit SensorClient(const Endpoint& crEndpoint);

    /**
     * @fn ~SensorClient
     * @brief Destructor
     */
    ~SensorClient() noexcept;

private:
    /**
     * @fn read
     * @brief Read with timeout.
     * @see Client#read
     */
    void read() override;

    /**
     * @fn connect
     * @see Client#connect
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
     * @see Client#stop
     */
    void stop() override;

    /**
     * @see Client#handleResolve
     */
    void handleResolve(const boost::system::error_code& crError,
                       boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept override;

    /**
     * @see Client#handleConnect
     */
    void handleConnect(const boost::system::error_code& crError,
                       boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept override;

    /// @var mTimeout
    /// Read timer
    boost::asio::deadline_timer mTimeout;
};

}  // namespace client
}  // namespace feed

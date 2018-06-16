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

#include "../../Defines.h"

#include "Client.h"

/// @namespace feed
namespace feed
{
/// @namespace client
namespace client
{
/**
 * @class GpsdClient
 * @brief Connect to a GPSD server.
 * @extends Client
 */
class GpsdClient : public Client
{
public:
    NON_COPYABLE(GpsdClient)

    /**
     * @fn GpsdClient
     * @brief Constructor
     * @param crHost The hostname
     * @param crPort The port
     * @param rFeed  The handler Feed reference
     */
    GpsdClient(const Endpoint& crEndpoint);

    /**
     * @fn ~GpsdClient
     * @brief Destructor
     */
    virtual ~GpsdClient() noexcept;

private:
    /**
     * @see Client#connect
     */
    void connect() override;

    /**
     * @fn stop
     * @brief Send unwatch-request before stop.
     * @see Client#stop
     */
    void stop() override;

    /**
     * @see Client#handleResolve
     */
    void
    handleResolve(const boost::system::error_code& crError,
                  boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept override;

    /**
     * @see Client#handleConnect
     */
    void
    handleConnect(const boost::system::error_code& crError,
                  boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept override;

    /**
     * @fn handleWatch
     * @brief Handler for watch-request sending
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleWatch(const boost::system::error_code& crError,
                     std::size_t vBytes) noexcept;
};

}  // namespace client
}  // namespace feed

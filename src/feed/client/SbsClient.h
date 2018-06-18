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

#include "../../Defines.h"
#include "Client.h"

/// @namespace feed
namespace feed
{
/// @namespace client
namespace client
{
/**
 * @class SbsClient
 * @brief Connect to a SBS server.
 * @extends Client
 */
class SbsClient : public Client
{
public:
    NON_COPYABLE(SbsClient)

    /**
     * @fn SbsClient
     * @brief Constructor
     * @param crHost  The hostname
     * @param crPort  The port
     * @param rFeed   The handler Feed reference
     */
    explicit SbsClient(const Endpoint& crEndpoint);

    /**
     * @fn ~SbsClient
     * @brief Destructor
     */
    virtual ~SbsClient() noexcept;

private:
    /**
     * @see Client#connect
     */
    void connect() override;

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
};

}  // namespace client
}  // namespace feed

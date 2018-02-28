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
namespace client
{
/**
 * @class SbsClient extends Client
 * @brief Handle connections to a SBS server.
 * @see Client.h
 */
class SbsClient : public Client
{
public:
    /**
     * Non-copyable
     */
    SbsClient(const SbsClient&) = delete;
    /**
     * Not assignable
     */
    SbsClient& operator=(const SbsClient&) = delete;
    /**
     * @fn SbsClient
     * @brief Constructor
     * @param cr_host  The hostname
     * @param cr_port  The port
     * @param cr_login The login string to transmit
     * @param r_feed   The handler Feed reference
     */
    SbsClient(const std::string& cr_host, const std::string& cr_port, feed::Feed& r_feed);
    /**
     * @fn ~SbsClient
     * @brief Destructor
     */
    virtual ~SbsClient() noexcept;

private:
    /**
     * @fn connect
     * @override Client::connect
     */
    void connect() override;
    /**
     * @fn handleResolve
     * @override Client::handleResolve
     */
    void handleResolve(const boost::system::error_code& cr_ec,
                       boost::asio::ip::tcp::resolver::iterator it) noexcept override;
    /**
     * @fn handleConnect
     * @override Client::handleConnect
     */
    void handleConnect(const boost::system::error_code& cr_ec,
                       boost::asio::ip::tcp::resolver::iterator it) noexcept override;
};

}  // namespace client
}  // namespace network

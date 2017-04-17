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

class APRSCClient: public Client
{
public:
    APRSCClient(const APRSCClient&) = delete;
    APRSCClient& operator=(const APRSCClient&) = delete;

    APRSCClient(boost::asio::signal_set& /*sigset*/, const std::string& /*host*/,
                const std::string& /*port*/, const std::string& /*login*/,
                Feed& /*feed*/);
    virtual ~APRSCClient() noexcept;

private:
    void connect() noexcept override;
    void process() noexcept override;

    void handleResolve(const boost::system::error_code& /*ec*/,
                       boost::asio::ip::tcp::resolver::iterator /*it*/) noexcept override;
    void handleConnect(const boost::system::error_code& /*ec*/,
                       boost::asio::ip::tcp::resolver::iterator /*it*/) noexcept override;
    /**
     * Send login string - handler
     */
    void handleLogin(const boost::system::error_code& /*ec*/, std::size_t /*s*/) noexcept;

    /**
     * Login string
     */
    std::string mLoginStr;
};

#endif /* SRC_TCP_CLIENT_APRSCCLIENT_H_ */

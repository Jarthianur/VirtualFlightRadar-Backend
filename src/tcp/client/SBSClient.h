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

#ifndef SRC_TCP_CLIENT_SBSCLIENT_H_
#define SRC_TCP_CLIENT_SBSCLIENT_H_

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <string>

#include "Client.h"

class SBSClient: public Client
{
public:
    SBSClient(const SBSClient&) = delete;
    SBSClient& operator=(const SBSClient&) = delete;

    SBSClient(boost::asio::signal_set& /*sigset*/, const std::string& /*host*/,
              const std::string& /*port*/, Feed& /*feed*/);
    virtual ~SBSClient() noexcept;

private:
    void connect() noexcept override;
    void process() noexcept override;

    void handleResolve(const boost::system::error_code& /*ec*/,
                       boost::asio::ip::tcp::resolver::iterator /*it*/) noexcept override;
    void handleConnect(const boost::system::error_code& /*ec*/,
                       boost::asio::ip::tcp::resolver::iterator /*it*/) noexcept override;

};

#endif /* SRC_TCP_CLIENT_SBSCLIENT_H_ */

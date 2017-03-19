/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#ifndef SRC_TCP_CLIENT_WINDCLIENT_H_
#define SRC_TCP_CLIENT_WINDCLIENT_H_

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <string>

#include "../../parser/WindParser.h"
#include "Client.h"
#include "../../config/Parameters.h"

#define WC_RCV_TIMEOUT WINDCLIENT_RECEIVE_TIMEOUT

class WindClient: public Client
{
public:
    WindClient(const WindClient&) = delete;
    WindClient& operator=(const WindClient&) = delete;

    WindClient(boost::asio::signal_set& /*sigset*/, const std::string& /*host*/,
               const std::string& /*port*/);
    virtual ~WindClient() noexcept;

private:
    void read() noexcept override;
    void process() noexcept override;
    void connect() noexcept override;
    void checkDeadline() noexcept;
    void stop() noexcept override;

    void handleResolve(const boost::system::error_code& /*ec*/,
                       boost::asio::ip::tcp::resolver::iterator /*it*/) noexcept override;
    void handleConnect(const boost::system::error_code& /*ec*/,
                       boost::asio::ip::tcp::resolver::iterator /*it*/) noexcept override;

    bool mStopped;
    boost::asio::deadline_timer mTimeout;
    WindParser mParser;
};

#endif /* SRC_TCP_CLIENT_WINDCLIENT_H_ */

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

#ifndef SRC_TCP_CLIENT_SENSORCLIENT_H_
#define SRC_TCP_CLIENT_SENSORCLIENT_H_

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <string>

#include "Client.h"
#include "../../config/Parameters.h"

#define WC_RCV_TIMEOUT WINDCLIENT_RECEIVE_TIMEOUT

class SensorClient: public Client
{
public:
    SensorClient(const SensorClient&) = delete;
    SensorClient& operator=(const SensorClient&) = delete;

    SensorClient(boost::asio::signal_set& /*sigset*/, const std::string& /*host*/,
                 const std::string& /*port*/, Feed& /*feed*/);
    virtual ~SensorClient() noexcept;

private:
    void read() noexcept override;
    void connect() noexcept override;
    /**
     * Check read timed out.
     */
    void checkDeadline() noexcept;
    void stop() noexcept override;

    void handleResolve(const boost::system::error_code& /*ec*/,
                       boost::asio::ip::tcp::resolver::iterator /*it*/) noexcept override;
    void handleConnect(const boost::system::error_code& /*ec*/,
                       boost::asio::ip::tcp::resolver::iterator /*it*/) noexcept override;

    /**
     * Client stopped
     */
    bool mStopped;
    /**
     * read timer
     */
    boost::asio::deadline_timer mTimeout;
};

#endif /* SRC_TCP_CLIENT_SENSORCLIENT_H_ */

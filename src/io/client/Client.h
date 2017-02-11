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

#ifndef CLIENT_H_
#define CLIENT_H_

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <cstddef>
#include <string>
#include "../../util/Parameters.h"

#define C_CON_WAIT_TIMEVAL CLIENT_CONNECT_WAIT_TIMEVAL

class Client
{
public:
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    virtual ~Client() throw ();

    void run();

protected:
    Client(boost::asio::signal_set& s, const std::string& host, const std::string& port,
            const std::string& comp);

    void awaitStop();
    void timedConnect();
    virtual void stop();
    virtual void read();
    virtual void process() = 0;
    virtual void connect() = 0;

    void handleTimedConnect(const boost::system::error_code& ec);
    void handleRead(const boost::system::error_code& ec, std::size_t s);
    virtual void handleResolve(const boost::system::error_code& ec,
            boost::asio::ip::tcp::resolver::iterator it) = 0;
    virtual void handleConnect(const boost::system::error_code& ec,
                    boost::asio::ip::tcp::resolver::iterator it) = 0;

    boost::asio::io_service io_service_;
    boost::asio::signal_set& signals_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;

    std::string response;
    boost::asio::streambuf buffer;
    const std::string host;
    const std::string port;
    const std::string component;

private:
    boost::asio::deadline_timer connect_timer;
};

#endif /* CLIENT_H_ */

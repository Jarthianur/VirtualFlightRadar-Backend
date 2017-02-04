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
#include <string>

// seconds to wait before reconnect
#define WAIT_TIMEVAL 30

class Client
{
public:
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    virtual ~Client() throw ();

    void run();

protected:
    Client(boost::asio::signal_set& s, const std::string& host, const std::string& port);

    virtual void awaitStop();
    virtual void connect() = 0;
    virtual void read() = 0;
    void reconnect();

    boost::asio::io_service io_service_;
    boost::asio::signal_set& signals_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;

    std::string response;
    boost::asio::streambuf buffer;
    const std::string host;
    const std::string port;

private:
    boost::asio::deadline_timer deadline_;
};

#endif /* CLIENT_H_ */

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

#ifndef NMEASERVER_H_
#define NMEASERVER_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <netinet/in.h>
#include <mutex>
#include <string>
#include <vector>

#include "Connection.h"

#define MAX_CLIENTS 5

class NMEAServer
{
public:
    NMEAServer(const NMEAServer&) = delete;
    NMEAServer& operator=(const NMEAServer&) = delete;

    NMEAServer(boost::asio::signal_set& s, in_port_t port);
    virtual ~NMEAServer() throw ();

    void run();
    void writeToAll(const std::string& msg);

private:
    void accept();
    void awaitStop();
    void stopAll();

    std::mutex mutex;

    boost::asio::io_service io_service_;
    boost::asio::signal_set& signals_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    std::vector<boost::shared_ptr<Connection>> clients;
};

#endif /* NMEASERVER_H_ */

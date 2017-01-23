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

#include "ConnectOutNMEA.h"

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <stddef.h>
#include <functional>
#include <iterator>

#include "Connection.h"
#include "Logger.h"

ConnectOutNMEA::ConnectOutNMEA(in_port_t port)
        : nmea_out(AF_INET, port)
{
    sin_s = sizeof(sockaddr);
}

ConnectOutNMEA::~ConnectOutNMEA()
{
    clients.clear();
}

void ConnectOutNMEA::listenOut()
{
    nmea_out.createSocket(AF_INET, SOCK_STREAM, 0);
    nmea_out.setSocketOpt(SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    nmea_out.fillAddr();
    nmea_out.listenToSocket(MAX_CLIENTS);
}

void ConnectOutNMEA::connectClient()
{
    auto client = std::make_shared<Connection>();
    client->setConSock(accept(nmea_out.getConSock(), client->getSockAddrPtr(), &sin_s));
    const std::string ip(inet_ntoa(client->con_addr.sin_addr));

    if (clients.size() >= MAX_CLIENTS)
    {
        Logger::warn("Cannot accept more connections, refused: ", std::ref(ip));
    }
    else if (client->getConSock() == C_SO_NOSET)
    {
        Logger::error("Accept connection failed to ", std::ref(ip));
    }
    else
    {
        Logger::info("Connection from ", std::ref(ip));
        clients.push_back(client);
    }
}

int ConnectOutNMEA::sendMsgOut(std::string& msg)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    for (size_t i = 0; i < clients.size(); ++i)
    {
        if (send(clients.at(i)->con_sock, msg.c_str(), msg.length(), MSG_NOSIGNAL) <= 0)
        {
            Logger::warn("Lost connection to ",
                         inet_ntoa(clients.at(i)->con_addr.sin_addr));
            clients.erase(clients.begin() + i);
        }
    }
    return clients.size();
}

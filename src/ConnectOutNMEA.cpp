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

#include "ConnectOutNMEA.h"
#include "Logger.h"
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

ConnectOutNMEA::ConnectOutNMEA(const int out_port)
: nmea_out_port(out_port)
{
}

ConnectOutNMEA::~ConnectOutNMEA()
{
    close();
}

int ConnectOutNMEA::listenOut()
{
    std::lock_guard<std::mutex> lock(this->mutex);

    if ((nmea_out.con_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        Logger::error("Creating socket for ", "NMEA-out");
        return -1;
    }

    if (setsockopt(nmea_out.con_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        Logger::error("Setting socketopt REUSEADDR on ", "NMEA-out");
        return -1;
    }

    memset(&nmea_out.con_addr, 0, sizeof(nmea_out.con_addr));
    nmea_out.con_addr.sin_family = AF_INET;
    nmea_out.con_addr.sin_port = htons(nmea_out_port);
    nmea_out.con_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(nmea_out.con_sock, (struct sockaddr*)&nmea_out.con_addr, sizeof(struct sockaddr)) == -1) {
        Logger::error("Binding socket for ", "NMEA-out");
        return -1;
    }

    if (listen(nmea_out.con_sock, MAX_CLIENTS) == -1) {
        Logger::error("Listening to socket for ", "NMEA-out");
        return -1;
    }
    Logger::info("Listening for clients at port ", std::to_string(nmea_out_port).c_str());

    return 0;
}

int ConnectOutNMEA::connectClient()
{
    unsigned int sin_s = sizeof(struct sockaddr);
    Connection client;
    client.con_sock = accept(nmea_out.con_sock, (struct sockaddr*)&client.con_addr, &sin_s);
    if (client.con_sock == -1 || clients.size() >= MAX_CLIENTS) {
        Logger::error("Accepting connection to ", inet_ntoa(client.con_addr.sin_addr));
        ::close(client.con_sock);
        return -1;
    } else {
        Logger::info("Connection from ", inet_ntoa(client.con_addr.sin_addr));
        clients.push_back(client);
    }
    return 0;
}

void ConnectOutNMEA::close()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    for (Connection& client : clients) {
        closeClient(client);
    }
    clients.clear();
    if (nmea_out.con_sock != -1) {
        ::close(nmea_out.con_sock);
        nmea_out.con_sock = -1;
    }
    return;
}

void ConnectOutNMEA::closeClient(Connection& client)
{
    if (client.con_sock == -1) {
        return;
    } else {
        ::close(client.con_sock);
        client.con_sock = -1;
        return;
    }
}

int ConnectOutNMEA::sendMsgOut(std::string& msg)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    for (unsigned int i = 0; i < clients.size(); ++i) {
        if (send(clients.at(i).con_sock, msg.c_str(), msg.length(), MSG_NOSIGNAL) <= 0) {
            Logger::warn("Lost connection to ", inet_ntoa(clients.at(i).con_addr.sin_addr));
            closeClient(std::ref(clients.at(i)));
            clients.erase(clients.begin() + i);
        }
    }
    return clients.size();
}

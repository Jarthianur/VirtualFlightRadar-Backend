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

#include "ConnectIn.h"
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdexcept>

ConnectIn::ConnectIn(const char* hostname, const int port, unsigned int to)
: in_hostname(hostname),
  in_port(port),
  timeout(to)
{
    memset(buffer,0,sizeof(buffer));
}

ConnectIn::~ConnectIn()
{
    close();
}

int ConnectIn::connectIn()
{
    if (::connect(in_con.con_sock, (struct sockaddr*) &in_con.con_addr, sizeof(struct sockaddr)) == -1) {
        std::cout << "Could not connect to server!" << std::endl;
        return -1;
    }
    std::cout << "Connected to server" << std::endl;
    return 0;
}

int ConnectIn::setupConnectIn()
{
    if ((in_host_info = gethostbyname(in_hostname)) == NULL) {
        std::cout << "Could not resolve Hostname!" << std::endl;
        return -1;
    }

    if ((in_con.con_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cout << "Could not create socket!" << std::endl;
        return -1;
    }

    if (timeout > 0) {
        struct timeval tv;
        tv.tv_sec = timeout;
        tv.tv_usec = 0;
        if (setsockopt(in_con.con_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval)) == -1) {
            std::cout << "Could not set socketopt RCVTIMEO!" << std::endl;
            return -1;
        }
    }

    memset(&in_con.con_addr, 0, sizeof(in_con.con_addr));
    in_con.con_addr.sin_family = AF_INET;
    in_con.con_addr.sin_port = htons(in_port);
    in_con.con_addr.sin_addr = *((struct in_addr*) in_host_info->h_addr);

    return 0;
}

int ConnectIn::readLineIn()
{
#define EOL "\r\n"

    int eol = linebuffer.find(EOL);
    int recv_l = 0;
    while (eol == -1) {
        if ((recv_l = recv(in_con.con_sock, buffer, BUFF_S-1, 0)) <= 0) {
            return -1;
        }
        buffer[recv_l] = 0;
        linebuffer.append(buffer);
        eol = linebuffer.find(EOL);
    }
    eol += 2;
    try {
        response = linebuffer.substr(0,eol);
        linebuffer.erase(0,eol);
    } catch (const std::out_of_range& e) {
        std::cout << e.what();
        return -1;
    }
    if (response.length() == 0) {
        return -1;
    }
    return response.length();
}

void ConnectIn::close()
{
    ::close(in_con.con_sock);
}

const std::string& ConnectIn::getResponse() const
{
    return response;
}

/*
Copyright_License {

  Copyright (C) 2016 Virtual Flight Radar - Backend
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

#include "ConnectInAPRSC.h"

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>

ConnectInAPRSC::ConnectInAPRSC(const char* ogn_host, int ogn_port, std::string& login)
: ConnectIn(ogn_host, ogn_port),
  login_str(login)
{
    login_str.append("\r\n");
}

ConnectInAPRSC::~ConnectInAPRSC()
{
}

int ConnectInAPRSC::connectIn()
{
    if (::connect(in_con.con_sock, (struct sockaddr*) &in_con.con_addr, sizeof(struct sockaddr)) == -1) {
        std::cout << "Could not connect to server!" << std::endl;
        return -1;
    }
    if (send(in_con.con_sock, login_str.c_str(), login_str.length(), 0) <= 0) {
        return -1;
    }
    //maybe verify correct login for ogn:
    //logresp USER verified

    std::cout << "Connected to server" << std::endl;
    return 0;
}

int ConnectInAPRSC::setupConnectIn()
{
    if ((in_host_info = gethostbyname(in_hostname)) == NULL) {
        std::cout << "Could not resolve Hostname!" << std::endl;
        return -1;
    }

    if ((in_con.con_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cout << "Could not create socket!" << std::endl;
        return -1;
    }

    if (setsockopt(in_con.con_sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) == -1) {
        std::cout << "Could not set socketopt KEEPALIVE!" << std::endl;
        return -1;
    }

    memset(&in_con.con_addr, 0, sizeof(in_con.con_addr));
    in_con.con_addr.sin_family = AF_INET;
    in_con.con_addr.sin_port = htons(in_port);
    in_con.con_addr.sin_addr = *((struct in_addr*) in_host_info->h_addr);
    return 0;
}

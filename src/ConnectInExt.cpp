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

#include "ConnectInExt.h"
#include "Logger.h"
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

ConnectInExt::ConnectInExt(const char* ogn_host, int ogn_port, std::string& login, unsigned int to)
: ConnectIn(ogn_host, ogn_port, to),
  login_str(login)
{
    login_str.append("\r\n");
}

ConnectInExt::~ConnectInExt()
{
}

int ConnectInExt::connectIn()
{
    if (::connect(in_con.con_sock, (struct sockaddr*) &in_con.con_addr, sizeof(struct sockaddr)) == -1) {
        Logger::error("Connecting to ", this->in_hostname);
        return -1;
    }

    if (send(in_con.con_sock, login_str.c_str(), login_str.length(), 0) <= 0) {
        Logger::error("Sending login to ", this->in_hostname);
        return -1;
    }
    //maybe verify correct login for ogn:
    //logresp USER verified

    Logger::info("Connected to ", this->in_hostname);
    return 0;
}

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

#include "ConnectInExt.h"

#include <sys/socket.h>

#include "ConnectionException.h"

ConnectInExt::ConnectInExt(const std::string& hostname, int port,
        const std::string& login, unsigned int to)
        : ConnectIn(hostname, port, to),
          login_str(login)
{
    login_str.append("\r\n");
}

ConnectInExt::~ConnectInExt()
{
}

void ConnectInExt::connectIn() throw (ConnectionException)
{
    in_con.connect(in_hostname);

    if (send(in_con.getConSock(), login_str.c_str(), login_str.length(), 0) <= 0)
    {
        throw ConnectionException(std::string("Failed to send login to ") + in_hostname);
    }
    //maybe verify correct login for ogn:
    //logresp USER verified
}

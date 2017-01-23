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

#include "InputConnection.h"

#include <sys/socket.h>

#include "ConnectionException.h"

#define IC_SO_CON_ERR -1

InputConnection::InputConnection(sa_family_t family, in_port_t port)
        : Connection(family, port)
{
}

InputConnection::~InputConnection()
{
}

void InputConnection::fillAddr(in_addr& addr)
{
    con_addr.sin_addr = addr;
}

void InputConnection::connect(const std::string& host)
{
    if (::connect(con_sock, getSockAddrPtr(), sizeof(sockaddr)) == IC_SO_CON_ERR)
    {
        throw ConnectionException(std::string("Cannot connect to ") + host);
    }
}

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

#include "Connection.h"
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

Connection::Connection(int family, int port)
{
    memset(&con_addr, 0, sizeof(con_addr));
    con_addr.sin_family = family;
    con_addr.sin_port = htons(port);
}
Connection::Connection()
{
}

void Connection::setConSock(int sock)
{
    if (con_sock == -1)
    {
        con_sock = sock;
    }
}

Connection::~Connection()
{
    close();
}

void Connection::createSocket(int family, int style, int protocol)
        throw (ConnectionException)
{
    if (con_sock == -1)
    {
        if ((con_sock = socket(family, style, protocol)) == -1)
        {
            throw ConnectionException("Cannot create socket");
        }
    }
}

void Connection::setSocketOpt(int level, int optname, void* optval, socklen_t optlen)
        throw (ConnectionException)
{
    if (setsockopt(con_sock, level, optname, optval, optlen) == -1)
    {
        throw ConnectionException("Cannot set socket option");
    }
}

int Connection::getConSock()
{
    return con_sock;
}

sockaddr* Connection::getSockAddrPtr()
{
    return (sockaddr*) &con_addr;
}

void Connection::close()
{
    if (con_sock != -1)
    {
        ::close(con_sock);
    }
}

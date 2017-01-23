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

#include "ConnectionException.h"

Connection::Connection(sa_family_t family, in_port_t port)
{
    memset(&con_addr, 0, sizeof(con_addr));
    con_addr.sin_family = family;
    con_addr.sin_port = htons(port);
}
Connection::Connection()
{
}

void Connection::setConSock(int32_t sock)
{
    if (con_sock == C_SO_NOSET)
    {
        con_sock = sock;
    }
}

Connection::~Connection()
{
    close();
}

void Connection::createSocket(sa_family_t family, int32_t style, int32_t protocol)
{
    if (con_sock == C_SO_NOSET)
    {
        if ((con_sock = socket(family, style, protocol)) == C_SO_CREAT_ERR)
        {
            throw ConnectionException("Cannot create socket");
        }
    }
}

void Connection::setSocketOpt(int32_t level, int32_t optname, void* optval,
        socklen_t optlen)
{
    if (setsockopt(con_sock, level, optname, optval, optlen) == C_SO_OPT_ERR)
    {
        throw ConnectionException("Cannot set socket option");
    }
}

int32_t Connection::getConSock()
{
    return con_sock;
}

sockaddr* Connection::getSockAddrPtr()
{
    return (sockaddr*) &con_addr;
}

void Connection::close()
{
    if (con_sock != C_SO_NOSET)
    {
        ::close(con_sock);
    }
}

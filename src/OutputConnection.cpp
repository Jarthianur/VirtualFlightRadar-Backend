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

#include "OutputConnection.h"

#include <netinet/in.h>
#include <sys/socket.h>

#include "ConnectionException.h"

OutputConnection::OutputConnection(int family, int port)
        : Connection(family, port)
{
}

OutputConnection::~OutputConnection()
{
}

void OutputConnection::fillAddr()
{
    con_addr.sin_addr.s_addr = INADDR_ANY;
}

void OutputConnection::listenToSocket(unsigned int max) throw (ConnectionException)
{
    if (bind(con_sock, (struct sockaddr*) &con_addr, sizeof(struct sockaddr)) == -1)
    {
        throw ConnectionException("Cannot bind socket");
    }
    if (listen(con_sock, max) == -1)
    {
        throw ConnectionException("Cannot listen to socket");
    }
}

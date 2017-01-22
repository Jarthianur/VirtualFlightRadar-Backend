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

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include "ConnectionException.h"

class Connection
{
public:
    Connection(int, int);
    Connection();
    virtual ~Connection() throw ();

    void createSocket(int, int, int) throw (ConnectionException);
    void setSocketOpt(int, int, void*, socklen_t) throw (ConnectionException);
    int getConSock();
    void close();

protected:
    void setConSock(int);
    sockaddr* getSockAddrPtr();
    sockaddr_in con_addr;
    int con_sock = -1;

    friend class ConnectOutNMEA;
};

#endif /* CONNECTION_H_ */

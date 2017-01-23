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
#include <sys/un.h>
#include <cstdint>

#define C_SO_NOSET -1
#define C_SO_CREAT_ERR C_SO_NOSET
#define C_SO_OPT_ERR C_SO_CREAT_ERR

class Connection
{
public:
    Connection(sa_family_t family, in_port_t port);
    Connection();
    virtual ~Connection() throw ();

    void createSocket(sa_family_t family, int32_t style, int32_t protocol);
    void setSocketOpt(int32_t level, int32_t optname, void* optval, socklen_t optlen);
    int32_t getConSock();
    void close();

protected:
    void setConSock(int32_t sock);
    sockaddr* getSockAddrPtr();
    sockaddr_in con_addr;
    int32_t con_sock = C_SO_NOSET;

    friend class ConnectOutNMEA;
};

#endif /* CONNECTION_H_ */

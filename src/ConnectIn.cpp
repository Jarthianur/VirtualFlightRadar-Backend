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

#include <asm-generic/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <cstring>
#include <stdexcept>

#include "ConnectionException.h"

ConnectIn::ConnectIn(const std::string& hostname, const int port, unsigned int to)
        : in_host_info(nullptr),
          in_con(AF_INET, port),
          in_hostname(hostname),
          timeout(to)

{
    memset(buffer, 0, sizeof(buffer));
}

ConnectIn::~ConnectIn()
{
}

void ConnectIn::connectIn() throw (ConnectionException)
{
    in_con.connect(in_hostname);
}

void ConnectIn::setupConnectIn() throw (ConnectionException)
{
    if ((in_host_info = gethostbyname(in_hostname.c_str())) == NULL)
    {
        throw ConnectionException(std::string("Cannot resolve hostname: ") + in_hostname);
    }
    in_con.createSocket(AF_INET, SOCK_STREAM, 0);
    in_con.setSocketOpt(SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));

    if (timeout > 0)
    {
        struct timeval tv;
        tv.tv_sec = timeout;
        tv.tv_usec = 0;
        in_con.setSocketOpt(SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(timeval));
    }
    in_con.fillAddr(*((in_addr*) in_host_info->h_addr));
}

int ConnectIn::readLineIn()
{
#define EOL "\r\n"

    int eol = linebuffer.find(EOL);
    int recv_l = 0;
    while (eol == -1)
    {
        if ((recv_l = recv(in_con.getConSock(), buffer, BUFF_S - 1, 0)) <= 0)
        {
            return -1;
        }
        buffer[recv_l] = 0;
        linebuffer.append(buffer);
        eol = linebuffer.find(EOL);
    }
    eol += 2;
    try
    {
        response = linebuffer.substr(0, eol);
        linebuffer.erase(0, eol);
    }
    catch (const std::out_of_range& e)
    {
        return MSG_READ_ERR;
    }
    if (response.length() == 0)
    {
        return MSG_READ_ERR;
    }
    return response.length();
}

const std::string& ConnectIn::getResponse() const
{
    return response;
}

void ConnectIn::close()
{
    in_con.close();
}

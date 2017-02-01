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

#ifndef CONNECTIN_H_
#define CONNECTIN_H_

#include <netinet/in.h>
#include <sys/types.h>
#include <cstdint>
#include <ctime>
#include <string>

#include "InputConnection.h"

struct hostent;

#define CI_BUFF_S 4096
#define CI_MSG_READ_ERR -1

class ConnectIn
{
public:
    /**
     * constructor with host,port
     */
    ConnectIn(const std::string& host, in_port_t port, time_t to = 0);
    virtual ~ConnectIn() throw ();

    /**
     * setup input socket
     */
    virtual void setupConnectIn();

    /**
     * connect to input service/server
     */
    virtual void connectIn();

    /**
     * read line from input socket
     */
    ssize_t readLineIn();

    void close();

    /**
     * getters/setters
     */
    const std::string& getResponse() const;

protected:
    /**
     * buffers
     */
    std::string response;
    std::string linebuffer;
    char buffer[CI_BUFF_S];

    /**
     * input connection stuff
     */
    hostent* in_host_info;
    InputConnection in_con;
    const std::string in_host;
    time_t timeout;

    int32_t yes = 1;
};

#endif /* CONNECTIN_H_ */

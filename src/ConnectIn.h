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

#ifndef CONNECTIN_H_
#define CONNECTIN_H_

#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include "Connection.h"

#define BUFF_S 4096

class ConnectIn
{
public:
    /**
     * constructor with host,port
     */
    ConnectIn(const char*, const int, unsigned int = 0);
    virtual ~ConnectIn() throw();

    /**
     * setup input socket
     * returns 0 on success, -1 on failure.
     */
    virtual int setupConnectIn();

    /**
     * connect to input service/server
     * returns 0 on success, -1 on failure.
     */
    virtual int connectIn();

    /**
     * close all sockets
     */
    void close();

    /**
     * read line from input socket
     */
    int readLineIn();

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
    char buffer[BUFF_S];

    /**
     * input connection stuff
     */
    struct hostent* in_host_info;
    Connection in_con;
    const char* in_hostname;
    const int in_port;
    unsigned int timeout;
};

#endif /* CONNECTIN_H_ */

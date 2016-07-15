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

#ifndef CONNECTOUTNMEA_H_
#define CONNECTOUTNMEA_H_

#include "Connection.h"
#include <sys/socket.h>
#include <string>
#include <vector>
#include <mutex>

#define MAX_CLIENTS 5

class ConnectOutNMEA
{
public:
    ConnectOutNMEA(const int);
    virtual ~ConnectOutNMEA() throw();

    /**
     * (all) methods are threadsafe.
     */

    /**
     * create output socket and listen to it
     */
    int listenOut();

    /**
     * wait for client to connect.
     * if MAX_CLIENTS connections are open, return -1,
     * else 0.
     * not threadsafe!
     */
    int connectClient();

    /**
     * close all sockets
     */
    void close();

    void closeClient(Connection&);

    /**
     * broadcast msg to all clients.
     * if a client is off, it will be removed and closed.
     * returns number of active clients.
     */
    int sendMsgOut(std::string&);

private:

    std::mutex mutex;

    /**
     * options
     */
    int yes = 1;
    int no = 0;

    /**
     * client, output connection
     */
    Connection nmea_out;
    std::vector<Connection> clients;

    /**
     * output port
     */
    const int nmea_out_port;
};

#endif /* CONNECTOUTNMEA_H_ */

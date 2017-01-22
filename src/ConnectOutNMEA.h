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

#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "OutputConnection.h"

#define MAX_CLIENTS 5

class ConnectOutNMEA
{
public:
    ConnectOutNMEA(const int);
    virtual ~ConnectOutNMEA() throw();

    /**
     * create output socket and listen to it
     */
    void listenOut() throw (ConnectionException);

    /**
     * wait for client to connect.
     * if MAX_CLIENTS connections are open, or any error occured
     * throw Exception.
     */
    void connectClient();

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
    OutputConnection nmea_out;
    std::vector<std::shared_ptr<Connection>> clients;

    /**
     * addr size
     */
    unsigned int sin_s;
};

#endif /* CONNECTOUTNMEA_H_ */

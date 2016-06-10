/*
 * ConnectOut.h
 *
 *  Created on: 18.04.2016
 *      Author: lula
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

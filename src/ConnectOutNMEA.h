/*
 * ConnectOut.h
 *
 *  Created on: 18.04.2016
 *      Author: lula
 */

#ifndef CONNECTOUTNMEA_H_
#define CONNECTOUTNMEA_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <mutex>

class ConnectOutNMEA
{
public:
    ConnectOutNMEA(const int);
    virtual ~ConnectOutNMEA();

    /**
     * create output socket and wait for client to connect
     */
    int listenOut();

    /**
     * wait for client to connect
     */
    int connectClient();

    /**
     * close all sockets
     */
    void close();

    void closeClient();

    /**
     * send msg to client socket
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
     * client address
     */
    struct sockaddr_in xcs_cli_addr;

    /**
     * nmea-out address
     */
    struct sockaddr_in nmea_out_addr;

    /**
     * client, output sockets
     */
    int nmea_out_sock;
    int xcs_cli_sock = -1;

    /**
     * output port
     */
    const int nmea_out_port;
};

#endif /* CONNECTOUTNMEA_H_ */

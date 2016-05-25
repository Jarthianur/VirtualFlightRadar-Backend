/*
 * Connector.h
 *
 *  Created on: 16.04.2016
 *      Author: lula
 */

#ifndef CONNECTIN_H_
#define CONNECTIN_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>

#define BUFF_S 4096

class ConnectIn
{
public:
    /**
     * constructor with host,port
     */
    ConnectIn(const char*, const int);
    virtual ~ConnectIn() throw();

    /**
     * setup input socket
     * returns 0 on success, -1 on failure.
     */
    virtual int setupConnectIn() = 0;

    /**
     * connect to input service/server
     * returns 0 on success, -1 on failure.
     */
    virtual int connectIn() = 0;

    /**
     * close all sockets
     */
    void close();

    /**
     * read line from input socket
     */
    int readLineIn(int);

    /**
     * getters/setters
     */
    const std::string& getResponse() const;

    int getInSock() const;

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
    struct sockaddr_in in_addr;
    int in_sock = 0;
    const char* in_hostname;
    const int in_port;

};

#endif /* CONNECTIN_H_ */

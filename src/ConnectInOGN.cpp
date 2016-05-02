/*
 * ConnectorOGN.cpp
 *
 *  Created on: 17.04.2016
 *      Author: lula
 */

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include "ConnectInOGN.h"

ConnectInOGN::ConnectInOGN(const char* ogn_host, int ogn_port, const char* callsgn, const char* pwd)
: ConnectIn(ogn_host, ogn_port),
  callsign(callsgn),
  passwd(pwd)
{
}

ConnectInOGN::~ConnectInOGN()
{
}

int ConnectInOGN::connectIn()
{
    //std::cout << "connect to ogn ..." << std::endl;

    if (::connect(in_sock, (struct sockaddr*) &in_addr, sizeof(struct sockaddr)) == -1) {
        //std::cout << "Could not connect to server!" << std::endl;
        return -1;
    }

    if (sendLogin() <= 0) {
        return -1;
    }

    //std::cout << "connected to OGN" << std::endl;
    return 0;
}

int ConnectInOGN::setupConnectIn()
{
    if ((in_host_info = gethostbyname(in_hostname)) == NULL) {
        //std::cout << "Could not resolve Hostname!" << std::endl;
        return -1;
    }

    if ((in_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        //std::cout << "Could not create socket!" << std::endl;
        return -1;
    }

    if (setsockopt(in_sock, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) == -1) {
        //std::cout << "Could not set socketopt KEEPALIVE!" << std::endl;
        return -1;
    }

    memset(&in_addr, 0, sizeof(in_addr));
    in_addr.sin_family = AF_INET;
    in_addr.sin_port = htons(in_port);
    in_addr.sin_addr = *((struct in_addr*) in_host_info->h_addr);

    //std::cout << "ogn port= " << ntohs(in_addr.sin_port) << std::endl;
    //std::cout << "ogn addr= " << inet_ntoa(in_addr.sin_addr) << std::endl;
    return 0;
}

int ConnectInOGN::sendLogin()
{
    std::string login_str = "user ";
    login_str.append(callsign);
    login_str.append(" pass ");
    login_str.append(passwd);
    //global ogn connect
    //login_str.append(" vers aprsc 2.0.18-ge7666c5 filter r/49.574325/9.395813/100");
    login_str.append("\r\n");
    return send(in_sock, login_str.c_str(), login_str.length(), 0);
}

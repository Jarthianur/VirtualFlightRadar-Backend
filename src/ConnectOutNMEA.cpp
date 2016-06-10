/*
 * ConnectOut.cpp
 *
 *  Created on: 18.04.2016
 *      Author: lula
 */

#include "ConnectOutNMEA.h"

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>

ConnectOutNMEA::ConnectOutNMEA(const int out_port)
: nmea_out_port(out_port)
{
}

ConnectOutNMEA::~ConnectOutNMEA()
{
    close();
}

int ConnectOutNMEA::listenOut()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    std::cout << "start nmea output socket..." << std::endl;

    if ((nmea_out.con_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cout << "Could not create socket!" << std::endl;
        return -1;
    }

    if (setsockopt(nmea_out.con_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        std::cout << "Could not set socketopt REUSEADDR!" << std::endl;
        return -1;
    }

    memset(&nmea_out.con_addr, 0, sizeof(nmea_out.con_addr));
    nmea_out.con_addr.sin_family = AF_INET;
    nmea_out.con_addr.sin_port = htons(nmea_out_port);
    nmea_out.con_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(nmea_out.con_sock, (struct sockaddr*)&nmea_out.con_addr, sizeof(struct sockaddr)) == -1) {
        std::cout << "Could not bind socket!" << std::endl;
        return -1;
    }

    if (listen(nmea_out.con_sock, MAX_CLIENTS) == -1) {
        std::cout << "Can not listen to socket!" << std::endl;
        return -1;
    }
    return 0;
}

int ConnectOutNMEA::connectClient()
{
    std::cout << "wait for client..."<< std::endl;
    unsigned int sin_s = sizeof(struct sockaddr);
    Connection client;
    client.con_sock = accept(nmea_out.con_sock, (struct sockaddr*)&client.con_addr, &sin_s);
    if (client.con_sock == -1 || clients.size() >= MAX_CLIENTS) {
        std::cout << "Could not accept connection!" << std::endl;
        return -1;
    } else {
        std::cout<< "connection from " << inet_ntoa(client.con_addr.sin_addr) <<std::endl;
        clients.push_back(client);
    }
    return 0;
}

void ConnectOutNMEA::close()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    for (Connection& client : clients) {
        closeClient(client);
    }
    clients.clear();
    if (nmea_out.con_sock != -1) {
        ::close(nmea_out.con_sock);
        nmea_out.con_sock = -1;
    }
    return;
}

void ConnectOutNMEA::closeClient(Connection& client)
{
    if (client.con_sock == -1) {
        return;
    } else {
        ::close(client.con_sock);
        client.con_sock = -1;
        return;
    }
}

int ConnectOutNMEA::sendMsgOut(std::string& msg)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    for (unsigned int i = 0; i < clients.size(); ++i) {
        //std::cout << "send to " << i << std::endl;
        if (send(clients.at(i).con_sock, msg.c_str(), msg.length(), MSG_NOSIGNAL) <= 0) {
            closeClient(std::ref(clients.at(i)));
            clients.erase(clients.begin() + i);
        }
    }
    return clients.size();
}

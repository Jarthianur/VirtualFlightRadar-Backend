/*
 * Connector.cpp
 *
 *  Created on: 16.04.2016
 *      Author: lula
 */

#include "ConnectIn.h"
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdexcept>

ConnectIn::ConnectIn(const char* hostname, const int port)
: in_hostname(hostname),
  in_port(port)
{
   memset(buffer,0,sizeof(buffer));
}

ConnectIn::~ConnectIn()
{
   close();
}

int ConnectIn::connectIn()
{
    std::cout << "connect to ... " << in_con.con_sock << std::endl;

    if (::connect(in_con.con_sock, (struct sockaddr*) &in_con.con_addr, sizeof(struct sockaddr)) == -1) {
        std::cout << "Could not connect to server!" << std::endl;
        return -1;
    }

    std::cout << "connected to " << in_con.con_sock << std::endl;
    return 0;
}

int ConnectIn::setupConnectIn()
{
    if ((in_host_info = gethostbyname(in_hostname)) == NULL) {
        std::cout << "Could not resolve Hostname!" << std::endl;
        return -1;
    }

    if ((in_con.con_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cout << "Could not create socket!" << std::endl;
        return -1;
    }

    memset(&in_con.con_addr, 0, sizeof(in_con.con_addr));
    in_con.con_addr.sin_family = AF_INET;
    in_con.con_addr.sin_port = htons(in_port);
    in_con.con_addr.sin_addr = *((struct in_addr*) in_host_info->h_addr);

    std::cout << "adsb port= " << ntohs(in_con.con_addr.sin_port) << std::endl;
    std::cout << "adsb addr= " << inet_ntoa(in_con.con_addr.sin_addr) << std::endl;
    return 0;
}

int ConnectIn::readLineIn()
{
#define EOL "\r\n"

   int eol = linebuffer.find(EOL);
   if (eol == -1) {
      if (recv(in_con.con_sock, buffer, BUFF_S-1, 0) <= 0) {
         return -1;
      }
      linebuffer.append(buffer);
      eol = linebuffer.find(EOL);
   }
   eol += 2;
   try {
      response = linebuffer.substr(0,eol);
   } catch (const std::out_of_range& e) {
      std::cout << e.what();
      return -1;
   }
   linebuffer.clear();
   if (response.length() == 0) {
      return -1;
   }
   return response.length();
}

void ConnectIn::close()
{
   ::close(in_con.con_sock);
}

const std::string& ConnectIn::getResponse() const
{
   return response;
}

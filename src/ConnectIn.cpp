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
: response(""),
  linebuffer(""),
  in_hostname(hostname),
  in_port(port)
{
   memset(buffer,0,sizeof(buffer));
}

ConnectIn::~ConnectIn()
{
   close();
}

int ConnectIn::readLineIn(int sock)
{
#define EOL "\r\n"

   int eol = linebuffer.find(EOL);
   if (eol == -1) {
      if (recv(sock, buffer, BUFF_S-1, 0) <= 0) {
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
   ::close(in_sock);
}

const std::string& ConnectIn::getResponse() const
{
   return response;
}

int ConnectIn::getInSock() const
{
    return in_sock;
}

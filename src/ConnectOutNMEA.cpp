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
#include <stdexcept>

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
   std::cout << "start nmea output socket..." << std::endl;

   if ((nmea_out_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      std::cout << "Could not create socket!" << std::endl;
      return -1;
   }

   if (setsockopt(nmea_out_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      std::cout << "Could not set socketopt REUSEADDR!" << std::endl;
      return -1;
   }

   memset(&nmea_out_addr, 0, sizeof(nmea_out_addr));
   nmea_out_addr.sin_family = AF_INET;
   nmea_out_addr.sin_port = htons(nmea_out_port);
   nmea_out_addr.sin_addr.s_addr = INADDR_ANY;

   if (bind(nmea_out_sock, (struct sockaddr*)&nmea_out_addr, sizeof(struct sockaddr)) == -1) {
      std::cout << "Could not bind socket!" << std::endl;
      return -1;
   }

   if (listen(nmea_out_sock, 5) == -1) {
      std::cout << "Can not listen to socket!" << std::endl;
      return -1;
   }
   return 0;
}

int ConnectOutNMEA::connectClient()
{
   int con = 0;

   while (con == 0) {
      std::cout << "wait for client..."<< std::endl;
      unsigned int sin_s = sizeof(struct sockaddr);
      xcs_cli_sock = accept(nmea_out_sock, (struct sockaddr*)&xcs_cli_addr, &sin_s);
      if (xcs_cli_sock == -1) {
         std::cout << "Could not accept connection!" << std::endl;
         //return -1;
      }

      std::cout<< "connection from " << inet_ntoa(xcs_cli_addr.sin_addr) <<std::endl;
      con = 1;
   }
   return 0;
}

void ConnectOutNMEA::close()
{
   ::close(xcs_cli_sock);
   ::close(nmea_out_sock);
   return;
}

int ConnectOutNMEA::sendMsgOut(std::string& msg) const
{
   return send(xcs_cli_sock, msg.c_str(), msg.length(), 0);
}

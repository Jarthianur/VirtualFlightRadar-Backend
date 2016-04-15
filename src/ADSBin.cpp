/*
 * Connector.cpp
 *
 *  Created on: 10.04.2016
 *      Author: lula
 */

#include "ADSBin.h"

ADSBin::ADSBin(const char* srchost, int srcport, int dstport)
: response(""),
  linebuffer(""),
  src_host(srchost),
  src_port(srcport),
  dst_port(dstport)
{
   memset(buffer,0,sizeof(buffer));
}

ADSBin::~ADSBin()
{
   close();
}

const char* ADSBin::getSrcHost() const
{
   return src_host;
}

void ADSBin::close()
{
   ::close(sockfd);
   ::close(dstsockfd);
   ::close(new_sockfd);
}

int ADSBin::readLine()
{
#define EOL "\r\n"
   int eol = linebuffer.find(EOL);
   if (eol == -1) {
      if (recv(sockfd, buffer, 2047, 0) == -1) {
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
   }
   linebuffer.clear();
   if (response.length() == 0) {
      return -2;
   }
   return response.length();
}

const int ADSBin::getSrcPort() const
{
   return src_port;
}

int ADSBin::connect()
{
   std::cout << "connect() ..." << std::endl;
   if ((host_info = gethostbyname(src_host)) == NULL) {
      std::cout << "Could not resolve Hostname!" << std::endl;
      return -1;
   }

   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      std::cout << "Could not create socket!" << std::endl;
      return -1;
   }

   memset(&address, 0, sizeof(address));
   address.sin_family = AF_INET;
   address.sin_port = htons(src_port);
   address.sin_addr = *((struct in_addr*) host_info->h_addr);

   std::cout << "address.fam= " << address.sin_family << std::endl;
   std::cout << "address.port= " << ntohs(address.sin_port) << std::endl;
   std::cout << "address.addr= " << inet_ntoa(dstaddress.sin_addr) << std::endl;
   if (::connect(sockfd, (struct sockaddr*) &address, sizeof(struct sockaddr)) == -1) {
      std::cout << "Could not connect to server!" << std::endl;
      close();
      return -1;
   }

   std::cout << "connected to ADS-B" << std::endl;

   // now try to accept connection from xcsoar
   std::cout << "connecting to xcsoar..." << std::endl;

   if ((dstsockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      std::cout << "Could not create socket!" << std::endl;
      return -1;
   }

   if (setsockopt(dstsockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      std::cout << "Could not set socketopt REUSEADDR!" << std::endl;
      return -1;
   }

   memset(&host_adr, 0, sizeof(host_adr));
   host_adr.sin_family = AF_INET;
   host_adr.sin_port = htons(dst_port);
   host_adr.sin_addr.s_addr = INADDR_ANY;

   if (bind(dstsockfd, (struct sockaddr*)&host_adr, sizeof(struct sockaddr)) == -1) {
      std::cout << "Could not bind socket!" << std::endl;
      return -1;
   }

   if (listen(dstsockfd, 20) == -1) {
      std::cout << "Could not listen to socket!" << std::endl;
      return -1;
   }
   int con = 0;
   //now wait for xcsoar to connect
   while (con == 0) {
      std::cout << "wait for xcsoar..."<< std::endl;
      unsigned int sin_s = sizeof(struct sockaddr);
      new_sockfd = accept(dstsockfd, (struct sockaddr*)&dstaddress, &sin_s);
      if (new_sockfd == -1) {
         std::cout << "Could not accept connection!" << std::endl;
         return -1;
      }
      char* addr = inet_ntoa(dstaddress.sin_addr);
      if (std::strcmp(addr, "127.0.0.1") != 0) {
         std::cout << "Do not accept connection from " << addr << std::endl;
         return -1;
      }
      std::cout<< "connection from " << addr <<std::endl;
      con = 1;
   }
   std::cout << "connected to xcsoar"<< std::endl;

   return 0;
}

int ADSBin::sendMsg(std::string& msg)
{
   if (send(new_sockfd, msg.c_str(), msg.length(), 0) > 0) {
      std::cout << "sent msg" << std::endl;
   }
   return 0;
}

const int ADSBin::getDstPort() const
{
   return dst_port;
}

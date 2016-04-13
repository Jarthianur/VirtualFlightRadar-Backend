/*
 * Connector.cpp
 *
 *  Created on: 10.04.2016
 *      Author: lula
 */

#include "ADSBin.h"

ADSBin::ADSBin(const char* host, int port)
: response(""),
  linebuffer(""),
  src_host(host),
  src_port(port)
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
      std::cout << e._M_msg;
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

   std::cout << "host= "<< host_info << std::endl;
   if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
      std::cout << "Could not create socket!" << std::endl;
      return -1;
   }

   std::cout << "socket= " << sockfd << std::endl;
   memset(&address, 0, sizeof(address));
   address.sin_family = AF_INET;
   address.sin_port = htons(src_port);
   address.sin_addr = *((struct in_addr*) host_info->h_addr);

   std::cout << "address.fam= " << address.sin_family << std::endl;
   std::cout << "address.port= " << ntohs(address.sin_port) << std::endl;
   std::cout << "address.addr= " << host_info->h_name << std::endl;
   if (::connect(sockfd, (struct sockaddr*) &address, sizeof(struct sockaddr)) == -1) {
      std::cout << "Could not connect to server!" << std::endl;
      close();
      return -1;
   }

   std::cout << "connected" << std::endl;
   return 0;
}

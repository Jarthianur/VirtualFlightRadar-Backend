/*
 * Connector.cpp
 *
 *  Created on: 10.04.2016
 *      Author: lula
 */

#include "ADSBin.h"

ADSBin::ADSBin(const std::string& host, int port)
: src_host(host),
  src_port(port),
  verbose(0),
  linebuffer(""),
  response("")
{

}

ADSBin::~ADSBin()
{
   close();
}

const std::string& ADSBin::getSrcHost() const
{
   return src_host;
}

void ADSBin::close()
{
   ::close(sockfd);
}

const int ADSBin::getSrcPort() const
{
   return src_port;
}

int ADSBin::connect()
{
   if ((host_info = gethostbyname(src_host.c_str())) == NULL) {
      std::cerr << "Could not resolve Hostname!" << std::endl;
      return -1;
   }

   if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
      std::cerr << "Could not create socket!" << std::endl;
      return -1;
   }

   memset(&address, 0, sizeof(address));
   address.sin_family = AF_INET;
   address.sin_port = htons(src_port);
   address.sin_addr = *((struct in_addr*) host_info->h_addr_list);

   if (::connect(sockfd, (struct sockaddr*) &address, sizeof(struct sockaddr)) == -1) {
      std::cerr << "Could not connect to server!" << std::endl;
      close();
      return -1;
   }

   return 0;
}

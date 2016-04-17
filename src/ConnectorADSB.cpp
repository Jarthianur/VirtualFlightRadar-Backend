#include "ConnectorADSB.h"
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>

ConnectorADSB::ConnectorADSB(const char* adsb_host, int adsb_port, int out_port)
: Connector(out_port),
  adsb_in_host(adsb_host),
  adsb_in_port(adsb_port)
{
}

ConnectorADSB::~ConnectorADSB()
{
   close();
}

void ConnectorADSB::close()
{
   ::close(adsb_in_sock);
   Connector::close();
}

int ConnectorADSB::connectIn()
{
   std::cout << "connect to adsb ..." << std::endl;
   if ((adsb_host_info = gethostbyname(adsb_in_host)) == NULL) {
      std::cout << "Could not resolve Hostname!" << std::endl;
      return -1;
   }

   if ((adsb_in_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      std::cout << "Could not create socket!" << std::endl;
      return -1;
   }

   memset(&adsb_in_addr, 0, sizeof(adsb_in_addr));
   adsb_in_addr.sin_family = AF_INET;
   adsb_in_addr.sin_port = htons(adsb_in_port);
   adsb_in_addr.sin_addr = *((struct in_addr*) adsb_host_info->h_addr);

   std::cout << "adsb port= " << ntohs(adsb_in_addr.sin_port) << std::endl;
   std::cout << "adsb addr= " << inet_ntoa(adsb_in_addr.sin_addr) << std::endl;
   if (::connect(adsb_in_sock, (struct sockaddr*) &adsb_in_addr, sizeof(struct sockaddr)) == -1) {
      std::cout << "Could not connect to server!" << std::endl;
      close();
      return -1;
   }

   std::cout << "connected to ADS-B" << std::endl;
   return 0;
}

int ConnectorADSB::getAdsbInSock() const
{
   return adsb_in_sock;
}

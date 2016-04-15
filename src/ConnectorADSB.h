#ifndef CONNECTORADSB_H_
#define CONNECTORADSB_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>

class ConnectorADSB
{
public:
   ConnectorADSB(const char*, int, int);
   virtual ~ConnectorADSB();

   int connect();
   void close();
   int readLine();
   int sendMsg(std::string&);

   const char* getSrcHost() const;
   const int getSrcPort() const;
   const int getDstPort() const;

   std::string response;
   std::string linebuffer;
   char buffer[2048];

   struct hostent* adsb_host_info;
   struct sockaddr_in adsb_in_addr;
   struct sockaddr_in xcs_cli_addr;
   struct sockaddr_in nmea_out_addr;
   int adsb_in_sock;
   int nmea_out_sock;
   int xcs_cli_sock;

private:
   const char* adsb_in_host;
   const int adsb_in_port;
   const int nmea_out_port;
   int yes = 1;
};

#endif /* CONNECTORADSB_H_ */

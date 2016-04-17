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
#include "Connector.h"

class ConnectorADSB : public Connector
{
public:
   ConnectorADSB(const char*, int, int);
   virtual ~ConnectorADSB();

   virtual int connectIn();
   virtual void close();
   virtual int readLineIn();

private:
   //adsb input stuff
   struct hostent* adsb_host_info;
   struct sockaddr_in adsb_in_addr;
   int adsb_in_sock;
   const char* adsb_in_host;
   const int adsb_in_port;
};

#endif /* CONNECTORADSB_H_ */

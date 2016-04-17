#ifndef CONNECTORADSB_H_
#define CONNECTORADSB_H_

#include "Connector.h"
#include <netdb.h>

class ConnectorADSB : public Connector
{
public:
   ConnectorADSB(const char*, int, int);
   virtual ~ConnectorADSB();

   int connectIn();
   void close();
   int getAdsbInSock() const;

private:
   /**
    * adsb input stuff
    */
   struct hostent* adsb_host_info;
   struct sockaddr_in adsb_in_addr;
   int adsb_in_sock;
   const char* adsb_in_host;
   const int adsb_in_port;
};

#endif /* CONNECTORADSB_H_ */

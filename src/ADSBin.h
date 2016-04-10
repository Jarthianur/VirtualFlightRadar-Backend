/*
 * Connector.h
 *
 *  Created on: 10.04.2016
 *      Author: lula
 */

#ifndef ADSBIN_H_
#define ADSBIN_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

class ADSBin
{
public:
   ADSBin(const std::string& host, int port);
   virtual ~ADSBin();

   int connect();
   void close();
   const std::string& getSrcHost() const;
   const int getSrcPort() const;

   int verbose;
   std::string device;
   std::string response;
   std::string linebuffer;

   struct hostent* host_info;
   struct sockaddr_in address;
   int sockfd;


private:
   const std::string src_host;
   const int src_port;
};

#endif /* ADSBIN_H_ */

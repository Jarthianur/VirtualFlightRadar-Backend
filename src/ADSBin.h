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
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdexcept>

class ADSBin
{
public:
   ADSBin(const char*, int, int);
   virtual ~ADSBin();

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

   struct hostent* host_info;
   struct sockaddr_in address;
   struct sockaddr_in dstaddress;
   struct sockaddr_in host_adr;
   int sockfd;
   int dstsockfd;
   int new_sockfd;

private:
   const char* src_host;
   const int src_port;
   const int dst_port;
   int yes = 1;
};

#endif /* ADSBIN_H_ */

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
   ADSBin(const char*, int, const char*, int);
   virtual ~ADSBin();

   int connect();
   void close();
   int readLine();
   int sendMsg(std::string&);

   const char* getSrcHost() const;
   const int getSrcPort() const;
   const char* getDstHost() const;
   const int getDstPort() const;

   std::string response;
   std::string linebuffer;
   char buffer[2048];

   struct hostent* host_info;
   struct hostent* dsthost_info;
   struct sockaddr_in address;
   struct sockaddr_in dstaddress;
   int sockfd;
   int dstsockfd;

private:
   const char* src_host;
   const int src_port;
   const char* dst_host;
   const int dst_port;
};

#endif /* ADSBIN_H_ */

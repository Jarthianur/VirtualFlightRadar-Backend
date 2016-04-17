/*
 * Connector.h
 *
 *  Created on: 16.04.2016
 *      Author: lula
 */

#ifndef CONNECTOR_H_
#define CONNECTOR_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

#define BUFF_S 4096

class Connector
{
public:
   Connector(const int);
   virtual ~Connector();

   //Connect to input service/server
   virtual int connectIn() = 0;
   //Create output socket and wait for client to connect
   virtual int connectOut();
   //Close all Sockets
   virtual void close();
   //Read line from input socket
   virtual int readLineIn() = 0;
   //send msg to Client Socket
   virtual int sendMsgOut(std::string&) const;

   //Getters/Setters
   const std::string& getResponse() const;

protected:
   //Buffers
      std::string response;
      std::string linebuffer;
      char buffer[BUFF_S];

private:
   //Client
   struct sockaddr_in xcs_cli_addr;
   //nmea-out service
   struct sockaddr_in nmea_out_addr;
   //Sockets
   int nmea_out_sock;
   int xcs_cli_sock;
   //nmea out port
   const int nmea_out_port;
   //options
   int yes = 1;
};

#endif /* CONNECTOR_H_ */

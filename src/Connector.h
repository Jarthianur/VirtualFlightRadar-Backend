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
   /**
    * constructor with output port
    */
   Connector(const int);
   virtual ~Connector();

   /**
    * connect to input service/server
    */
   virtual int connectIn() = 0;
   /**
    * create output socket and wait for client to connect
    */
   int connectOut();
   /**
    * wait for client to connect
    */
   int connectClient();
   /**
    * close all sockets
    */
   virtual void close();
   /**
    * read line from input socket
    */
   int readLineIn(int);
   /**
    * send msg to client socket
    */
   int sendMsgOut(std::string&) const;

   /**
    * getters/setters
    */
   const std::string& getResponse() const;

protected:
   /**
    * buffers
    */
   std::string response;
   std::string linebuffer;
   char buffer[BUFF_S];

private:
   /**
    * client address
    */
   struct sockaddr_in xcs_cli_addr;
   /**
    * nmea-out service
    */
   struct sockaddr_in nmea_out_addr;
   /**
    * client, output sockets
    */
   int nmea_out_sock;
   int xcs_cli_sock;
   /**
    * output port
    */
   const int nmea_out_port;
   /**
    * options
    */
   int yes = 1;
};

#endif /* CONNECTOR_H_ */

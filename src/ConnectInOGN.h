/*
 * ConnectorOGN.h
 *
 *  Created on: 17.04.2016
 *      Author: lula
 */

#ifndef CONNECTINOGN_H_
#define CONNECTINOGN_H_

#include "ConnectIn.h"

class ConnectInOGN: public ConnectIn
{
public:
   ConnectInOGN(const char*, const int, const char*, const char*);
   virtual ~ConnectInOGN();

   int setupConnectIn();
   int connectIn();

   int getOgnInSock() const;

private:
   /**
    * OGN specific login stuff
    */
   int sendLogin();
   const char* callsign;
   const char* passwd;

   /**
    * options
    */
   int yes = 1;
   int no = 0;

};

#endif /* CONNECTINOGN_H_ */

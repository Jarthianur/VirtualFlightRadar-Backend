/*
 * ConnectorOGN.h
 *
 *  Created on: 17.04.2016
 *      Author: lula
 */

#ifndef CONNECTINEXT_H_
#define CONNECTINEXT_H_

#include "ConnectIn.h"

class ConnectInExt: public ConnectIn
{
public:
   ConnectInExt(const char*, const int, std::string&);
   virtual ~ConnectInExt() throw();

   int setupConnectIn();
   int connectIn();

private:
   /**
    * login stuff
    */
   std::string login_str;

   /**
    * options
    */
   int yes = 1;
   int no = 0;

};

#endif /* CONNECTINEXT_H_ */

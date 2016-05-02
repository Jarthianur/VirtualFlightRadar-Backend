#ifndef CONNECTINADSB_H_
#define CONNECTINADSB_H_

#include "ConnectIn.h"

class ConnectInADSB : public ConnectIn
{
public:
   ConnectInADSB(const char*, const int);
   virtual ~ConnectInADSB();

   int setupConnectIn();
   int connectIn();

};

#endif /* CONNECTINADSB_H_ */

/*
 * ParserOGN.h
 *
 *  Created on: 17.04.2016
 *      Author: lula
 */

#ifndef PARSEROGN_H_
#define PARSEROGN_H_

#include "Parser.h"
#include "ExtendedAircraft.h"

class ParserOGN: public Parser
{
public:
   ParserOGN();
   virtual ~ParserOGN();

   int unpack(Aircraft&, const std::string&) const;
   void process(Aircraft&, std::string&, long double, long double, int);

private:
   long double dmsToDeg(long double dms) const;
   //constants
   long double kts2kmh = 1.852L;
   long double kmh2kts = 0.539957L;
   long double feet2m = 0.3048L;
   long double ms2fpm = 196.85L;
   long double fpm2ms = 0.00508L;
};

#endif /* PARSEROGN_H_ */

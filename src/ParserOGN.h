/*
 * ParserOGN.h
 *
 *  Created on: 17.04.2016
 *      Author: lula
 */

#ifndef PARSEROGN_H_
#define PARSEROGN_H_

#include "Parser.h"
#include <vector>

class ParserOGN: public Parser
{
public:
   ParserOGN(long double, long double, int);
   virtual ~ParserOGN();

   Aircraft* unpack(const std::string&);
   void process(Aircraft*, std::string&);

private:
   void splitToTokens(const std::string&);
   long double dmsToDeg(long double dms) const;
   //constants
   long double kts2kmh = 1.852L;
   long double kmh2kts = 0.539957L;
   long double feet2m = 0.3048L;
   long double ms2fpm = 196.85L;
   long double fpm2ms = 0.00508L;
   //comment tokens
   std::vector<std::string> tokens;
};

#endif /* PARSEROGN_H_ */

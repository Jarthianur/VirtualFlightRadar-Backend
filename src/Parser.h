/*
 * Parser.h
 *
 *  Created on: 17.04.2016
 *      Author: lula
 */

#ifndef PARSER_H_
#define PARSER_H_

#define BUFF_OUT_S 4096

#include <string>
#include "Aircraft.h"

class Parser
{
public:
   Parser();
   virtual ~Parser();

   virtual int unpack(Aircraft&, const std::string&) const = 0;
   virtual void process(Aircraft&, std::string&, long double, long double, int) = 0;

protected:
   char buffer[BUFF_OUT_S];
   long double radian(long double) const;
   long double degree(long double) const;
   int ldToI(long double) const;
   int checksum(const char*) const;
   /**
    * Number PI as precise as CPU can do
    */
   const long double PI;

};

#endif /* PARSER_H_ */

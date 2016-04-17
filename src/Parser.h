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

   /**
    * unpack a msg into Aircraft
    */
   virtual int unpack(Aircraft&, const std::string&) const = 0;

   /**
    * build nmea-msg from Aircraft to given string-reference
    */
   virtual void process(Aircraft&, long double, long double, int) = 0;

protected:
   /**
    * format string buffer
    */
   char buffer[BUFF_OUT_S];

   /**
    * convert degree to radian
    */
   long double radian(long double) const;

   /**
    * convert radian to degree
    */
   long double degree(long double) const;

   /**
    * convert long double to int, round to nearest number
    */
   int ldToI(long double) const;

   /**
    * compute checksum of nmea string
    */
   int checksum(const char*) const;

   /**
    * Number PI as precise as CPU can do
    */
   const long double PI;

};

#endif /* PARSER_H_ */

/*
 * Parser.cpp
 *
 *  Created on: 17.04.2016
 *      Author: lula
 */

#include "Parser.h"
#include <cmath>

Parser::Parser()
: PI(std::acos(-1.0L))
{
}

Parser::~Parser()
{
}

int Parser::checksum(const char* sentence) const
{
   int csum = 0;
   int i=1;
   while (sentence[i] != '*') {
      csum ^= (int) sentence[i++];
   }
   return csum;
}

long double Parser::radian(long double deg) const
{
   return ((deg * PI) / 180.0L);
}

long double Parser::degree(long double rad) const
{
   return (rad * 180.0L) / PI;
}

int Parser::ldToI(long double ld) const
{
   return (int)(ld + 0.5L);
}

/*
 * ADSBparser.h
 *
 *  Created on: 13.04.2016
 *      Author: lula
 */

#ifndef ADSBPARSER_H_
#define ADSBPARSER_H_

#include <string>

class ADSBparser
{
public:
   ADSBparser();
   virtual ~ADSBparser();

   void unpack(Aircraft&, std::string&);
};

#endif /* ADSBPARSER_H_ */

/*
 * Aircraft.h
 *
 *  Created on: 10.04.2016
 *      Author: lula
 */

#ifndef AIRCRAFT_H_
#define AIRCRAFT_H_

#include <string>

class Aircraft
{
public:
   Aircraft();
   virtual ~Aircraft();

   std::string id;
   long double latitude;
   long double longitude;
   int altitude;

   std::string nmea_str;

private:

};

#endif /* AIRCRAFT_H_ */

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
   double latitude;
   double longitude;
   int altitude;
   const std::string& toString();

private:

};

#endif /* AIRCRAFT_H_ */

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
   Aircraft(const std::string& id);
   virtual ~Aircraft();

   double getAltitude() const;
   void setAltitude(double altitude);
   const std::string& getId() const;
   double getLatitude() const;
   void setLatitude(double latitude);
   double getLongitude() const;
   void setLongitude(double longitude);

private:
   const std::string id;
   double latitude;
   double longitude;
   double altitude;
};

#endif /* AIRCRAFT_H_ */

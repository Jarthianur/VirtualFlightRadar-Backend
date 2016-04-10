/*
 * Aircraft.cpp
 *
 *  Created on: 10.04.2016
 *      Author: lula
 */

#include "Aircraft.h"

Aircraft::Aircraft(const std::string& id)
: id(id),
  latitude(0.0),
  longitude(0.0),
  altitude(NULL)
{
}

Aircraft::~Aircraft()
{
}

double Aircraft::getAltitude() const
{
   return altitude;
}

void Aircraft::setAltitude(double altitude)
{
   this->altitude = altitude;
}

const std::string& Aircraft::getId() const
{
   return id;
}

double Aircraft::getLatitude() const
{
   return latitude;
}

void Aircraft::setLatitude(double latitude)
{
   this->latitude = latitude;
}

double Aircraft::getLongitude() const
{
   return longitude;
}

void Aircraft::setLongitude(double longitude)
{
   this->longitude = longitude;
}

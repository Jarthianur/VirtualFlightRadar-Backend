/*
 * Aircraft.cpp
 *
 *  Created on: 10.04.2016
 *      Author: lula
 */

#include "Aircraft.h"

Aircraft::Aircraft()
: id(""),
  latitude(0.0),
  longitude(0.0),
  altitude(0)
{
}

Aircraft::~Aircraft()
{
}

const std::string& Aircraft::toString()
{
   return std::string("Aircraft "+id+", alt="+std::to_string(altitude)+", lat="+std::to_string(latitude)+", long="+std::to_string(longitude));
}

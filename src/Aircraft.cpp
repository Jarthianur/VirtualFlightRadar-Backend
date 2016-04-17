/*
 * Aircraft.cpp
 *
 *  Created on: 10.04.2016
 *      Author: lula
 */

#include "Aircraft.h"

Aircraft::Aircraft()
: id(""),
  latitude(0.0L),
  longitude(0.0L),
  altitude(0),
  nmea_str("")
{
}

Aircraft::~Aircraft()
{
}

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
  track(0.0),
  ground_speed(0),
  address_type(0),
  aircraft_type(0),
  climb_rate(0),
  valid(0)
{
}

Aircraft::~Aircraft()
{
}

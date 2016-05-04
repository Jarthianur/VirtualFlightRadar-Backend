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
  heading(0.0),
  gnd_speed(0),
  addr_type(0),
  aircraft_type(0),
  climb_rate(0),
  valid(0)
{
}

Aircraft::~Aircraft()
{
}

Aircraft::Aircraft(std::string& id, long double lat,
        long double lon, int alt)
: id(id),
  latitude(lat),
  longitude(lon),
  altitude(alt),
  valid(0)
{
}

Aircraft::Aircraft(std::string& id, long double lat,
        long double lon, int alt, float heading, int gnd_spd, int addr_t,
        int ac_t, int climb_r)
: Aircraft(id, lat, lon, alt)
{
    this->heading = heading;
    this->gnd_speed = gnd_spd;
    this->addr_type = addr_t;
    this->aircraft_type = ac_t;
    this->climb_rate = climb_r;
}

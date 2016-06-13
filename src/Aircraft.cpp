/*
Copyright_License {

  Copyright (C) 2016 Virtual Flight Radar - Backend
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License version 3
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "Aircraft.h"

Aircraft::~Aircraft()
{
}

Aircraft::Aircraft(std::string& id, long double lat,
        long double lon, int alt)
: id(id),
  latitude(lat),
  longitude(lon),
  altitude(alt)
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

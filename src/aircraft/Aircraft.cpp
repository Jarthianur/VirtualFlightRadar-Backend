/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

Aircraft::Aircraft(std::string& id, double lat, double lon, std::int32_t alt)
        : id(id),
          full_info(false),
          latitude(lat),
          longitude(lon),
          altitude(alt)
{
}

Aircraft::Aircraft(std::string& id, double lat, double lon, std::int32_t alt, double gnd_spd,
                   std::uint32_t id_t, std::int32_t ac_t, double climb_r, double turn_r, double heading)
        : Aircraft(id, lat, lon, alt)
{
    this->climb_rate = climb_r;
    this->turn_rate = turn_r;
    this->heading = heading;
    this->gnd_speed = gnd_spd;
    this->id_type = id_t;
    this->aircraft_type = ac_t;
}

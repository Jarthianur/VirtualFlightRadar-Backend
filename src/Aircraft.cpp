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
#include "Math.h"

Aircraft::~Aircraft()
{
}

Aircraft::Aircraft(std::string& id, long double lat, long double lon, int alt)
: id(id)
{
    Position pos(lat, lon, alt);
    this->addPosition(pos);
}

Aircraft::Aircraft(std::string& id, long double lat,
        long double lon, int alt, unsigned int heading, int gnd_spd, unsigned int id_t,
        int ac_t, float climb_r)
: Aircraft(id, lat, lon, alt)
{
    this->heading = heading;
    this->gnd_speed = gnd_spd;
    this->id_type = id_t;
    this->aircraft_type = ac_t;
    this->climb_rate = climb_r;
}

void Aircraft::addPosition(Position& pos)
{
    positions[last_pos] = pos;
    last_pos = (last_pos+1) % HISTORY_S;
    nr_of_pos++;
    return;
}

void Aircraft::calcMoveData()
{
    if (nr_of_pos < 2) return;
    Position& before = positions[(last_pos-1) % HISTORY_S];
    Position& last = positions[last_pos];
    heading = Math::ldToI(
            std::fmod(
                    Math::degree(
                            std::atan2(std::sin(before.longitude - last.longitude) * std::cos(before.latitude),
                                    std::cos(last.latitude) * std::sin(before.latitude) -
                                    std::sin(last.latitude) * std::cos(before.latitude) *
                                    std::cos(before.longitude - last.longitude))
                    ) + 360.0L, 360.0L)
    );

    if (aircraft_type == MIN_DATA) aircraft_type = UNKNOWN_T;
    return;
}

const Position& Aircraft::getLastPosition()
{
    return positions[last_pos];
}

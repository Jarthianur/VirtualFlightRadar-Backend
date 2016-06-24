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

Aircraft::Aircraft(std::string& id, long double lat, long double lon, int alt, int time)
: id(id)
{
    Position pos(lat, lon, alt, time);
    this->addPosition(pos);
}

Aircraft::Aircraft(std::string& id, long double lat,
        long double lon, int alt, int gnd_spd, unsigned int id_t,
        int ac_t, float climb_r, float turn_r, int time, int heading)
: Aircraft(id, lat, lon, alt, time)
{
    Position& pos = this->positions[last_pos];
    pos.climb_rate = climb_r;
    pos.turn_rate = turn_r;
    pos.heading = heading;
    this->gnd_speed = gnd_spd;
    this->id_type = id_t;
    this->aircraft_type = ac_t;
}

void Aircraft::addPosition(Position& pos)
{
    if (nr_of_pos != 0) {
        last_pos = (last_pos+1) % HISTORY_S;
    }
    positions[last_pos] = pos;
    nr_of_pos++;
    return;
}

Position& Aircraft::getLastPosition()
{
    return positions[last_pos];
}

Position& Aircraft::getBeforeLastPosition()
{
    return (nr_of_pos < 2) ? positions[last_pos] : positions[(last_pos + (HISTORY_S - 1)) % HISTORY_S];
}

bool Aircraft::isPosEvaluable()
{
    return (nr_of_pos > 1);
}

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

#include "Position.h"

Position::Position()
{
}

Position::Position(long double lat, long double lon, int alt)
: latitude(lat),
  longitude(lon),
  altitude(alt)
{
}

Position::Position(long double lat, long double lon, int alt,
        unsigned int track, float climb_r, float turn_r)
: Position(lat, lon, alt)
{
    this->heading = track;
    this->climb_rate = climb_r;
    this->turn_rate = turn_r;
}

Position::~Position()
{
}

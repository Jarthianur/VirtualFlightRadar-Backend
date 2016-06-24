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

#ifndef SRC_POSITION_H_
#define SRC_POSITION_H_

class Position
{
public:
    Position();
    Position(long double, long double, int);
    Position(long double, long double, int, unsigned int, float, float);
    virtual ~Position();

    /**
     * absolue position
     */
    long double latitude = 0.0L;
    long double longitude = 0.0L;
    int altitude = 0;
    unsigned int heading = 0;
    /**
     * relative to base
     */
    int distance = 0;
    /**
     * relative to last position
     */
    float climb_rate = 0.0;
    float turn_rate = 0.0;
};

#endif /* SRC_POSITION_H_ */

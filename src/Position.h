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
    Position(double, double, int, int);
    Position(double, double, int, int, double, double, double);
    virtual ~Position();

    /**
     * absolue position
     */
    double latitude = 0.0;
    double longitude = 0.0;
    // m
    int altitude = 0;
    // deg [0-359]
    double heading = 0;
    /**
     * relative to base
     */
    // m
    int distance = 0;
    /**
     * relative to last position
     */
    // m/s
    double climb_rate = 0.0;
    // deg/s
    double turn_rate = 0.0;
    // 0...hhmmss
    int timestamp = 0;
};

#endif /* SRC_POSITION_H_ */

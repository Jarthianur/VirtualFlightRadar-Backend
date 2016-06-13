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

#ifndef AIRCRAFT_H_
#define AIRCRAFT_H_

#include <string>

class Aircraft
{
public:
    Aircraft(std::string&, long double, long double, int);
    Aircraft(std::string&, long double, long double, int, float, int, int, int, int);
    virtual ~Aircraft() throw();

    /**
     * self explanatory
     */
    std::string id;
    long double latitude;
    long double longitude;
    int altitude;
    float heading = 0.0;
    int gnd_speed = 0;
    int addr_type = 0;
    //illegal ac_type == -1 used for check extended data
    int aircraft_type = -1;
    int climb_rate = 0;
    //0 = valid; +x(cycles) = invalid
    int valid = 0;

};

#endif /* AIRCRAFT_H_ */

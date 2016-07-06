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

#define UNKNOWN_T 0
#define ADSB_T 8
#define MIN_DATA -1
#define VALUE_NA -1000.0

class Aircraft
{
public:
    Aircraft(std::string&, double, double, int);
    Aircraft(std::string&, double, double, int, double, unsigned int, int, double, double, double);
    virtual ~Aircraft() throw();

    /**
     * self explanatory
     */
    std::string id;
    // m/s
    double gnd_speed = 0;
    unsigned int id_type = 1;
    int aircraft_type = MIN_DATA;
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
     * relative to last position
     */
    // m/s
    double climb_rate = 0.0;
    // deg/s
    double turn_rate = 0.0;

    //0 = valid; +x(cycles) = invalid
    int valid = 0;

};

#endif /* AIRCRAFT_H_ */

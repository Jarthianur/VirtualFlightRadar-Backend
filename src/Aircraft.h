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
#include "Position.h"

#define UNKNOWN_T 0
#define MIN_DATA -1
#define HISTORY_S 3

class Aircraft
{
public:
    Aircraft(std::string&, long double, long double, int);
    Aircraft(std::string&, long double, long double, int, unsigned int, int, unsigned int, int, float);
    virtual ~Aircraft() throw();

    /**
     * self explanatory
     */
    std::string id;
    unsigned int heading = 0;
    int gnd_speed = 0;
    float climb_rate = 0.0;
    unsigned int id_type = 0;
    int aircraft_type = MIN_DATA;
    //0 = valid; +x(cycles) = invalid
    int valid = 0;

    void addPosition(Position&);
    const Position& getLastPosition();

private:
    Position positions[HISTORY_S];
    void calcMoveData();

    unsigned int last_pos = 0;
    unsigned int nr_of_pos = 0;

};

#endif /* AIRCRAFT_H_ */

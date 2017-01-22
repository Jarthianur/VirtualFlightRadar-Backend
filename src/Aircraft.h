/*
Copyright_License {

  Copyright (C) 2017 Virtual Flight Radar - Backend
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
#include "Interpolation.h"

#define UNKNOWN_T 0
#define ADSB_T 8
#define MIN_DATA -1
#define HISTORY_S 2
#define VALUE_NA -1000.0

#define SPEED_FLAG 1
#define CLIMB_FLAG 2
#define TURN_FLAG  4
#define HEADING_FLAG 8
// since turn rate is only used internally, its flag may not be set
#define FULL_DATA 11

class Aircraft
{
public:
    Aircraft(std::string&, double, double, int, int);
    Aircraft(std::string&, double, double, int, double, unsigned int, int, double, double, int, double);
    virtual ~Aircraft() throw();

    /**
     * self explanatory
     */
    std::string id;
    // m/s
    double gnd_speed = 0;
    unsigned int id_type = 1;
    int aircraft_type = MIN_DATA;
    //0 = valid; +x(cycles) = invalid
    int valid = 0;
    /*
     * specify which fields are given from input; no need to calculate them
     */
    int data_flags = 0;
    /**
     * this stores movement values,
     * calculated with position history.
     */
    Interpolation interpolation;

    void addPosition(Position&);
    Position& getLastPosition();
    Position& getBeforeLastPosition();
    bool isPosEvaluable();

private:
    Position positions[HISTORY_S];

    unsigned int last_pos = 0;
    unsigned int nr_of_pos = 0;

};

#endif /* AIRCRAFT_H_ */

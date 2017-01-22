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

#ifndef AIRCRAFTPROCESSOR_H_
#define AIRCRAFTPROCESSOR_H_

#include <string>
#include "Aircraft.h"
class Aircraft;

#define BUFF_OUT_S 4096
#define LESS_BUFF_S 128


class AircraftProcessor
{
public:
    AircraftProcessor(double, double, int, double);
    virtual ~AircraftProcessor();

    /**
     * build nmea-msg from Aircraft into target string-reference
     */
    std::string process(Aircraft&);

    /**
     * build GPRMC and GPGGA
     */
    void gpsfix(std::string&);

private:
    /**
     * format string buffer
     */
    char buffer[BUFF_OUT_S];

    /**
     * compute checksum of nmea string
     */
    int checksum(const char*) const;

    /**
     * calculate relative position to base and absolute heading
     */
    void calcRelPosToBase(Aircraft&);

    /**
     * base position info
     */
    double baselat,
    baselong,
    /**
     * Latitude degree, minutes
     * Longitude degree, minutes
     */
    lat_deg = 0.0,
    lat_min = 0.0,
    long_deg = 0.0,
    long_min = 0.0,
    /**
     * Longitude base, Aircraft
     * Latitude base, Aircraft
     */
    long_b = 0.0,
    long_ac = 0.0,
    lat_b = 0.0,
    lat_ac = 0.0,
    /**
     * Longitude, Latitude distance
     */
    long_dist = 0.0,
    lat_dist = 0.0,
    /**
     * relative bearing, absolute bearing
     */
    bearing_rel = 0.0,
    bearing_abs = 0.0,
    /**
     * values to calculate distance
     */
    a = 0.0,
    /**
     * geoid separation
     */
    basegeoid;
    /**
     * (alt = height + antennaheight)
     */
    int basealt,
    /**
     * relative North, East, Vertical
     */
    rel_N = 0,
    rel_E = 0,
    rel_V = 0,
    /**
     * distance from base position to Aircraft
     */
    dist = 0;
    /**
     * Latitude: S - N
     * Longitude: W - E
     */
    char latstr = 'n', longstr = 'w';
};

#endif /* AIRCRAFTPROCESSOR_H_ */

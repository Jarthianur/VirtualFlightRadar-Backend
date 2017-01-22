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

#define BUFF_OUT_S 4096
#define LESS_BUFF_S 128

#include "Aircraft.h"

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
    lat_deg,
    lat_min,
    long_deg,
    long_min,
    /**
     * Longitude base, Aircraft
     * Latitude base, Aircraft
     */
    long_b,
    long_ac,
    lat_b,
    lat_ac,
    /**
     * Longitude, Latitude distance
     */
    long_dist,
    lat_dist,
    /**
     * relative bearing, absolute bearing
     */
    bearing_rel,
    bearing_abs,
    /**
     * values to calculate distance
     */
    a,
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
    rel_N,
    rel_E,
    rel_V,
    /**
     * distance from base position to Aircraft
     */
    dist;
    /**
     * Latitude: S - N
     * Longitude: W - E
     */
    char latstr, longstr;
};

#endif /* AIRCRAFTPROCESSOR_H_ */

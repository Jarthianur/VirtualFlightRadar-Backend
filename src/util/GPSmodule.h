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

#ifndef SRC_UTIL_GPSMODULE_H_
#define SRC_UTIL_GPSMODULE_H_

#include <cstdint>
#include <string>

#define GPSM_BUFF_S 8191
#define GPSM_L_BUFF_S 128

class GPSmodule
{
public:
    GPSmodule(double /*b_lat*/, double /*b_lon*/, std::int32_t /*b_alt*/, double /*geoid*/);
    virtual ~GPSmodule() noexcept;

    /**
     * build GPRMC and GPGGA
     */
    std::string gpsfix();

private:
    double mBaseLat, mBaseLong, mBaseGeoid,
    /**
     * Latitude degree, minutes
     * Longitude degree, minutes
     */
    mLatDeg, mLatMin, mLongDeg, mLongMin;
    std::int32_t mBaseAlt;

    /**
     * Latitude: S - N
     * Longitude: W - E
     */
    char mLatStr, mLongStr;

    /**
     * format string mBuffer
     */
    char mBuffer[GPSM_BUFF_S + 1];

};

#endif /* SRC_UTIL_GPSMODULE_H_ */

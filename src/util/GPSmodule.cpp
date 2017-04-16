/*
 Copyright_License {

 Copyright (C) 2016 VirtualFlightRadar-Backend
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

#include "GPSmodule.h"

#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "Math.hpp"

GPSmodule::GPSmodule(double b_lat, double b_lon, std::int32_t b_alt, double geoid)
        : mBaseLat(b_lat),
          mBaseLong(b_lon),
          mBaseGeoid(geoid),
          mBaseAlt(b_alt)
{
    mLatStr = (mBaseLat < 0) ? 'S' : 'N';
    mLongStr = (mBaseLong < 0) ? 'W' : 'E';
    mLatDeg = std::abs(std::floor(mBaseLat));
    mLatMin = std::abs(60.0 * (mBaseLat - mLatDeg));
    mLongDeg = std::abs(std::floor(mBaseLong));
    mLongMin = std::abs(60.0 * (mBaseLong - mLongDeg));
}

GPSmodule::~GPSmodule() noexcept
{
}

std::string GPSmodule::gpsfix()
{
    std::string nmea_str;
    std::int32_t csum;
    std::time_t now = std::time(0);
    std::tm* utc = std::gmtime(&now);

    //gprmc
    std::snprintf(
            mBuffer,
            GPSM_BUFF_S,
            "$GPRMC,%02d%02d%02d,A,%02.0lf%05.2lf,%c,%03.0lf%05.2lf,%c,0,0,%02d%02d%02d,001.0,W*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, mLatDeg, mLatMin, mLatStr, mLongDeg,
            mLongMin, mLongStr, utc->tm_mday, utc->tm_mon + 1, utc->tm_year - 100);
    csum = Math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, GPSM_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    //gpgga
    std::snprintf(
            mBuffer,
            GPSM_BUFF_S,
            "$GPGGA,%02d%02d%02d,%02.0lf%06.4lf,%c,%03.0lf%07.4lf,%c,1,05,1,%d,M,%.1lf,M,,*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, mLatDeg, mLatMin, mLatStr, mLongDeg,
            mLongMin, mLongStr, mBaseAlt, mBaseGeoid);
    csum = Math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, GPSM_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    return nmea_str;
}

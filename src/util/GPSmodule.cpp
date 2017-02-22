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

#include "GPSmodule.h"

#include <cmath>
#include <complex>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "Math.h"

GPSmodule::GPSmodule(double b_lat, double b_lon, int32_t b_alt, double geoid)
        : baselat(b_lat),
          baselong(b_lon),
          basegeoid(geoid),
          basealt(b_alt)
{
    latstr = (baselat < 0) ? 'S' : 'N';
    longstr = (baselong < 0) ? 'W' : 'E';
    lat_deg = std::abs(std::floor(baselat));
    lat_min = std::abs(60.0 * (baselat - lat_deg));
    long_deg = std::abs(std::floor(baselong));
    long_min = std::abs(60.0 * (baselong - long_deg));
}

GPSmodule::~GPSmodule()
{
}

std::string GPSmodule::gpsfix()
{
    std::string nmea_str;
    int32_t csum;

    time_t now = time(0);
    tm* utc = gmtime(&now);
    //gprmc
    snprintf(
            buffer,
            GPSM_BUFF_S - 1,
            "$GPRMC,%02d%02d%02d,A,%02.0lf%05.2lf,%c,%03.0lf%05.2lf,%c,0,0,%02d%02d%02d,001.0,W*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, latstr, long_deg,
            long_min, longstr, utc->tm_mday, utc->tm_mon + 1, utc->tm_year - 100);
    csum = Math::checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, GPSM_L_BUFF_S - 1, "%02x\r\n", csum);
    nmea_str.append(buffer);
    //gpgga
    snprintf(
            buffer,
            GPSM_BUFF_S - 1,
            "$GPGGA,%02d%02d%02d,%02.0lf%06.4lf,%c,%03.0lf%07.4lf,%c,1,05,1,%d,M,%.1lf,M,,*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, latstr, long_deg,
            long_min, longstr, basealt, basegeoid);
    csum = Math::checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, GPSM_L_BUFF_S - 1, "%02x\r\n", csum);
    nmea_str.append(buffer);

    return nmea_str;
}

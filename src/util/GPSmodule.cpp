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
#include <cstdio>
#include <ctime>

#include "Math.hpp"

GPSmodule::GPSmodule()
{
}

GPSmodule::~GPSmodule() noexcept
{
}

std::string GPSmodule::ggafix(double b_lat, double b_lon, std::int32_t b_alt,
                              double geoid)
{
    std::string nmea_str;
    std::int32_t csum;
    std::time_t now = std::time(0);
    std::tm* utc = std::gmtime(&now);

    char lat_str = (b_lat < 0) ? 'S' : 'N';
    char long_str = (b_lon < 0) ? 'W' : 'E';
    double lat_deg = std::abs(std::floor(b_lat));
    double lat_min = std::abs(60.0 * (b_lat - lat_deg));
    double long_deg = std::abs(std::floor(b_lon));
    double long_min = std::abs(60.0 * (b_lon - long_deg));

    std::snprintf(
            mBuffer,
            GPSM_BUFF_S,
            "$GPGGA,%02d%02d%02d,%02.0lf%06.4lf,%c,%03.0lf%07.4lf,%c,1,05,1,%d,M,%.1lf,M,,*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, lat_str, long_deg,
            long_min, long_str, b_alt, geoid);
    csum = Math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, GPSM_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    return nmea_str;
}

std::string GPSmodule::rmcfix(double b_lat, double b_lon, std::int32_t b_alt,
                              double geoid)
{
    std::string nmea_str;
    std::int32_t csum;
    std::time_t now = std::time(0);
    std::tm* utc = std::gmtime(&now);

    char lat_str = (b_lat < 0) ? 'S' : 'N';
    char long_str = (b_lon < 0) ? 'W' : 'E';
    double lat_deg = std::abs(std::floor(b_lat));
    double lat_min = std::abs(60.0 * (b_lat - lat_deg));
    double long_deg = std::abs(std::floor(b_lon));
    double long_min = std::abs(60.0 * (b_lon - long_deg));

    std::snprintf(
            mBuffer,
            GPSM_BUFF_S,
            "$GPRMC,%02d%02d%02d,A,%02.0lf%05.2lf,%c,%03.0lf%05.2lf,%c,0,0,%02d%02d%02d,001.0,W*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, lat_str, long_deg,
            long_min, long_str, utc->tm_mday, utc->tm_mon + 1, utc->tm_year - 100);
    csum = Math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, GPSM_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    return nmea_str;
}

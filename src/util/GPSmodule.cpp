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
#include <cstdint>
#include <cstdio>
#include <ctime>

#include "../data/Position.hpp"
#include "Math.hpp"

GPSmodule::GPSmodule()
{
}

GPSmodule::~GPSmodule() noexcept
{
}

std::string GPSmodule::ggafix(const struct ExtGPSPosition& pos)
{
    std::string nmea_str;
    std::int32_t csum;
    std::time_t now = std::time(0);
    std::tm* utc = std::gmtime(&now);

    char lat_str = (pos.position.latitude < 0) ? 'S' : 'N';
    char long_str = (pos.position.longitude < 0) ? 'W' : 'E';
    double lat_deg = std::abs(std::floor(pos.position.latitude));
    double lat_min = std::abs(60.0 * (pos.position.latitude - lat_deg));
    double long_deg = std::abs(std::floor(pos.position.longitude));
    double long_min = std::abs(60.0 * (pos.position.longitude - long_deg));

    std::snprintf(mBuffer,
    GPSM_BUFF_S,
            "$GPGGA,%02d%02d%02d,%02.0lf%07.4lf,%c,%03.0lf%07.4lf,%c,%1d,%02d,1,%d,M,%.1lf,M,,*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, lat_str,
            long_deg, long_min, long_str, pos.fixQa, pos.nrSats,
            pos.position.altitude, pos.geoid);
    csum = Math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, GPSM_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    return nmea_str;
}

std::string GPSmodule::rmcfix(const struct ExtGPSPosition& pos)
{
    std::string nmea_str;
    std::int32_t csum;
    std::time_t now = std::time(0);
    std::tm* utc = std::gmtime(&now);

    char lat_str = (pos.position.latitude < 0) ? 'S' : 'N';
    char long_str = (pos.position.longitude < 0) ? 'W' : 'E';
    double lat_deg = std::abs(std::floor(pos.position.latitude));
    double lat_min = std::abs(60.0 * (pos.position.latitude - lat_deg));
    double long_deg = std::abs(std::floor(pos.position.longitude));
    double long_min = std::abs(60.0 * (pos.position.longitude - long_deg));

    std::snprintf(mBuffer,
    GPSM_BUFF_S,
            "$GPRMC,%02d%02d%02d,A,%02.0lf%05.2lf,%c,%03.0lf%05.2lf,%c,0,0,%02d%02d%02d,001.0,W*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, lat_str,
            long_deg, long_min, long_str, utc->tm_mday, utc->tm_mon + 1,
            utc->tm_year - 100);
    csum = Math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, GPSM_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    return nmea_str;
}

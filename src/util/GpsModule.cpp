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

#include "GpsModule.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <ctime>

#include "../util/Position.hpp"
#include "Math.hpp"

namespace util
{

GpsModule::GpsModule()
{
}

GpsModule::~GpsModule() noexcept
{
}

std::string GpsModule::genGpggaStr(const struct ExtGPSPosition& cr_pos)
{
    std::string nmea_str;
    std::int32_t csum;
    std::time_t now = std::time(0);
    std::tm* utc = std::gmtime(&now);

    char lat_str = (cr_pos.position.latitude < 0) ? 'S' : 'N';
    char long_str = (cr_pos.position.longitude < 0) ? 'W' : 'E';
    double lat_deg = std::abs(std::floor(cr_pos.position.latitude));
    double lat_min = std::abs(60.0 * (cr_pos.position.latitude - lat_deg));
    double long_deg = std::abs(std::floor(cr_pos.position.longitude));
    double long_min = std::abs(60.0 * (cr_pos.position.longitude - long_deg));
// As we use XCSoar as frontend, we need to set the fix quality to 1. It doesn't support others.
    std::snprintf(mBuffer,
    GPSM_BUFF_S,
    /*"$GPGGA,%02d%02d%02d,%02.0lf%07.4lf,%c,%03.0lf%07.4lf,%c,%1d,%02d,1,%d,M,%.1lf,M,,*"*/
    "$GPGGA,%02d%02d%02d,%02.0lf%07.4lf,%c,%03.0lf%07.4lf,%c,1,%02d,1,%d,M,%.1lf,M,,*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, lat_str, long_deg,
            long_min, long_str, /*pos.fixQa,*/cr_pos.nrSats, cr_pos.position.altitude,
            cr_pos.geoid);
    csum = util::math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, GPSM_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    return nmea_str;
}

std::string GpsModule::genGprmcStr(const struct ExtGPSPosition& cr_pos)
{
    std::string nmea_str;
    std::int32_t csum;
    std::time_t now = std::time(0);
    std::tm* utc = std::gmtime(&now);

    char lat_str = (cr_pos.position.latitude < 0) ? 'S' : 'N';
    char long_str = (cr_pos.position.longitude < 0) ? 'W' : 'E';
    double lat_deg = std::abs(std::floor(cr_pos.position.latitude));
    double lat_min = std::abs(60.0 * (cr_pos.position.latitude - lat_deg));
    double long_deg = std::abs(std::floor(cr_pos.position.longitude));
    double long_min = std::abs(60.0 * (cr_pos.position.longitude - long_deg));

    std::snprintf(mBuffer,
    GPSM_BUFF_S,
            "$GPRMC,%02d%02d%02d,A,%02.0lf%05.2lf,%c,%03.0lf%05.2lf,%c,0,0,%02d%02d%02d,001.0,W*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, lat_str, long_deg,
            long_min, long_str, utc->tm_mday, utc->tm_mon + 1, utc->tm_year - 100);
    csum = util::math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, GPSM_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    return nmea_str;
}

}  // namespace util

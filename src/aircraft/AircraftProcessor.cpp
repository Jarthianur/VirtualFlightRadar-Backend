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

#include "AircraftProcessor.h"

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <cstdio>
#include <ctime>

#include "../main/VFRB.h"
#include "../util/ClimateData.h"
#include "../util/Configuration.h"
#include "../util/Math.h"
#include "Aircraft.h"

AircraftProcessor::AircraftProcessor(double b_lat, double b_long, int32_t b_alt,
        double geo)
        : baselat(b_lat),
          baselong(b_long),
          basegeoid(geo),
          basealt(b_alt)
{
}

AircraftProcessor::~AircraftProcessor()
{
}

int AircraftProcessor::checksum(const char* sentence) const
{
    int32_t csum = 0;
    size_t i = 1;
    while (sentence[i] != '*' && sentence[i] != '\0')
    {
        csum ^= (int) sentence[i++];
    }
    return csum;
}

std::string AircraftProcessor::process(Aircraft& ac)
{
    calcRelPosToBase(ac);

    if (dist > Configuration::filter_maxDist)
    {
        return "";
    }

    std::string nmea_str;

    //PFLAU
    snprintf(buffer, AP_BUFF_S - 1, "$PFLAU,,,,1,0,%d,0,%d,%d,%s*",
             Math::dToI(bearing_rel), rel_V, dist, ac.id.c_str());
    int32_t csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, AP_L_BUFF_S - 1, "%02x\r\n", csum);
    nmea_str.append(buffer);
    //PFLAA
    if (ac.aircraft_type == A_MIN_DATA)
    {
        snprintf(buffer, AP_BUFF_S - 1, "$PFLAA,0,%d,%d,%d,1,%s,,,,,8*", rel_N, rel_E,
                 rel_V, ac.id.c_str());
    }
    else
    {
        snprintf(buffer, AP_BUFF_S - 1, "$PFLAA,0,%d,%d,%d,%u,%s,%03d,,%d,%3.1lf,%1x*",
                 rel_N, rel_E, rel_V, ac.id_type, ac.id.c_str(), Math::dToI(ac.heading),
                 Math::dToI(ac.gnd_speed * Math::ms2kmh), ac.climb_rate,
                 ac.aircraft_type);
    }
    csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, AP_L_BUFF_S - 1, "%02x\r\n", csum);
    nmea_str.append(buffer);
    return nmea_str;
}

void AircraftProcessor::calcRelPosToBase(Aircraft& ac)
{
    long_b = Math::radian(baselong);
    long_ac = Math::radian(ac.longitude);
    lat_b = Math::radian(baselat);
    lat_ac = Math::radian(ac.latitude);
    long_dist = long_ac - long_b;
    lat_dist = lat_ac - lat_b;

    a = std::pow(std::sin(lat_dist / 2.0), 2.0) + std::cos(lat_b)
            * std::cos(lat_ac) * std::pow(std::sin(long_dist / 2.0), 2.0);
    dist = Math::dToI(6371000.0 * (2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a))));

    bearing_rel = Math::degree(
            std::atan2(
                    std::sin(long_ac - long_b) * std::cos(lat_ac),
                    std::cos(lat_b) * std::sin(lat_ac) - std::sin(lat_b)
                            * std::cos(lat_ac) * std::cos(long_ac - long_b)));
    bearing_abs = std::fmod((bearing_rel + 360.0), 360.0);

    rel_N = Math::dToI(std::cos(Math::radian(bearing_abs)) * dist);
    rel_E = Math::dToI(std::sin(Math::radian(bearing_abs)) * dist);
    rel_V = ac.qne ?
            ac.altitude - Math::calcIcaoHeight(VFRB::climate_data.getPress()) :
            ac.altitude - basealt;
}

std::string AircraftProcessor::gpsfix()
{
    std::string nmea_str;
    int32_t csum;

    latstr = (baselat < 0) ? 'S' : 'N';
    longstr = (baselong < 0) ? 'W' : 'E';
    lat_deg = std::abs(std::floor(baselat));
    lat_min = std::abs(60.0 * (baselat - lat_deg));
    long_deg = std::abs(std::floor(baselong));
    long_min = std::abs(60.0 * (baselong - long_deg));

    time_t now = time(0);
    tm* utc = gmtime(&now);
    //gprmc
    snprintf(
            buffer,
            AP_BUFF_S - 1,
            "$GPRMC,%02d%02d%02d,A,%02.0lf%05.2lf,%c,%03.0lf%05.2lf,%c,0,0,%02d%02d%02d,001.0,W*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, latstr, long_deg,
            long_min, longstr, utc->tm_mday, utc->tm_mon + 1, utc->tm_year - 100);
    csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, AP_L_BUFF_S - 1, "%02x\r\n", csum);
    nmea_str.append(buffer);
    //gpgga
    snprintf(
            buffer,
            AP_BUFF_S - 1,
            "$GPGGA,%02d%02d%02d,%02.0lf%06.4lf,%c,%03.0lf%07.4lf,%c,1,05,1,%d,M,%.1lf,M,,*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, latstr, long_deg,
            long_min, longstr, basealt, basegeoid);
    csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, AP_L_BUFF_S - 1, "%02x\r\n", csum);
    nmea_str.append(buffer);

    return nmea_str;
}

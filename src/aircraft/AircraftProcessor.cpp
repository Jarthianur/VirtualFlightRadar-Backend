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

#include <cmath>
#include <complex>
#include <cstdio>

#include "../config/Configuration.h"
#include "../data/ClimateData.h"
#include "../util/Math.hpp"
#include "../vfrb/VFRB.h"
#include "Aircraft.h"

AircraftProcessor::AircraftProcessor()
        : baselat(0.0),
          baselong(0.0),
          basealt(0)
{
}

AircraftProcessor::AircraftProcessor(double b_lat, double b_long, std::int32_t b_alt)
        : baselat(b_lat),
          baselong(b_long),
          basealt(b_alt)
{
    long_b = Math::radian(baselong);
    lat_b = Math::radian(baselat);
}

AircraftProcessor::~AircraftProcessor()
{
}

void AircraftProcessor::init(double lat, double lon, std::int32_t alt)
{
    baselat = lat;
    baselong = lon;
    basealt = alt;
    long_b = Math::radian(baselong);
    lat_b = Math::radian(baselat);
}

std::string AircraftProcessor::process(Aircraft &ac)
{
    calcRelPosToBase(ac);

    if (dist > Configuration::filter_maxDist)
    {
        return "";
    }

    std::string nmea_str;

    //PFLAU
    std::snprintf(buffer, AP_BUFF_S, "$PFLAU,,,,1,0,%d,0,%d,%d,%s*",
                  Math::dToI(bearing_rel), rel_V, dist, ac.id.c_str());
    std::int32_t csum = Math::checksum(buffer);
    nmea_str.append(buffer);
    std::snprintf(buffer, AP_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(buffer);

    //PFLAA
    if (!ac.full_info)
    {
        std::snprintf(buffer, AP_BUFF_S, "$PFLAA,0,%d,%d,%d,1,%s,,,,,%1x*", rel_N, rel_E,
                      rel_V, ac.id.c_str(), ac.aircraft_type);
    }
    else
    {
        std::snprintf(buffer, AP_BUFF_S, "$PFLAA,0,%d,%d,%d,%u,%s,%03d,,%d,%3.1lf,%1x*",
                      rel_N, rel_E, rel_V, ac.id_type, ac.id.c_str(),
                      Math::dToI(ac.heading), Math::dToI(ac.gnd_speed * Math::ms2kmh),
                      ac.climb_rate, ac.aircraft_type);
    }
    csum = Math::checksum(buffer);
    nmea_str.append(buffer);
    std::snprintf(buffer, AP_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(buffer);

    return nmea_str;
}

void AircraftProcessor::calcRelPosToBase(Aircraft &ac)
{
    long_ac = Math::radian(ac.longitude);
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
    rel_V = ac.qne_altitude ?
            ac.altitude - Math::calcIcaoHeight(VFRB::climate_data.getPress()) :
            ac.altitude - basealt;
}

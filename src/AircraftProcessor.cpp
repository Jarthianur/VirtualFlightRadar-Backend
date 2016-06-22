/*
Copyright_License {

  Copyright (C) 2016 Virtual Flight Radar - Backend
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
#include "Configuration.h"
#include <cstdio>
#include <ctime>
#include "Math.h"

AircraftProcessor::AircraftProcessor(long double b_lat, long double b_long, int b_alt, float geo)
: baselat(b_lat),
  baselong(b_long),
  basealt(b_alt),
  basegeoid(geo)
{
}

AircraftProcessor::~AircraftProcessor()
{
}

int AircraftProcessor::checksum(const char* sentence) const
{
    int csum = 0;
    int i=1;
    while (sentence[i] != '*' && sentence[i] != '\0') {
        csum ^= (int) sentence[i++];
    }
    return csum;
}

std::string AircraftProcessor::process(Aircraft& ac)
{
    calcPosInfo(ac);

    if (dist > Configuration::filter_maxDist) {
        return "";
    }

    std::string nmea_str;

    //PFLAU
    snprintf(buffer, BUFF_OUT_S, "$PFLAU,,,,1,0,%d,0,%d,%u,%s*", Math::ldToI(bearing_rel),
            Math::ldToI(rel_V), Math::ldToI(dist), ac.id.c_str());
    int csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, LESS_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(buffer);
    //PFLAA
    if (ac.aircraft_type == MIN_DATA) {
        snprintf(buffer, BUFF_OUT_S, "$PFLAA,0,%d,%d,%d,1,%s,,,,,8*", Math::ldToI(rel_N),
                Math::ldToI(rel_E), Math::ldToI(rel_V), ac.id.c_str());
    } else {
        snprintf(buffer, BUFF_OUT_S, "$PFLAA,0,%d,%d,%d,%u,%s,%03u,,%d,%3.1f,%1x*", Math::ldToI(rel_N),
                Math::ldToI(rel_E), Math::ldToI(rel_V), ac.id_type, ac.id.c_str(), ac.heading, ac.gnd_speed, ac.climb_rate, ac.aircraft_type);
    }
    csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, LESS_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(buffer);
    return nmea_str;
}

void AircraftProcessor::calcPosInfo(Aircraft& ac)
{
    const Position& ac_pos = ac.getLastPosition();
    long_b = Math::radian(baselong);
    long_ac = Math::radian(ac_pos.longitude);
    lat_b = Math::radian(baselat);
    lat_ac = Math::radian(ac_pos.latitude);
    long_dist = long_ac - long_b;
    lat_dist = lat_ac -lat_b;

    a = std::pow(std::sin(lat_dist/2.0L),2.0L) + std::cos(lat_b) * std::cos(lat_ac) * std::pow(std::sin(long_dist/2.0L),2.0L);
    c = 2.0L * std::atan2(std::sqrt(a), std::sqrt(1.0L - a));
    dist = 6371000.0L * c;

    bearing = std::atan2(std::sin(long_ac-long_b)*std::cos(lat_ac), std::cos(lat_b)*std::sin(lat_ac)-std::sin(lat_b)*std::cos(lat_ac)*std::cos(long_ac-long_b));
    bearing_rel = Math::degree(bearing);
    bearing_abs = std::fmod((bearing_rel + 360.0L), 360.0L);

    rel_N = std::cos(Math::radian(bearing_abs)) * dist;
    rel_E = std::sin(Math::radian(bearing_abs)) * dist;
    rel_V = (ac_pos.altitude * Math::feet2m) - basealt;
    return;
}

void AircraftProcessor::gprmc(std::string& nmea_str)
{
    nmea_str.clear();
    int csum;

    latstr = (baselat < 0)? 'S' : 'N';
    longstr = (baselong < 0)? 'W' : 'E';
    lat_deg = std::abs(std::floor(baselat));
    lat_min = std::abs(60.0L * (baselat - lat_deg));
    long_deg = std::abs(std::floor(baselong));
    long_min = std::abs(60.0L * (baselong - long_deg));

    time_t now = time(0);
    tm* utc = gmtime(&now);
    snprintf(buffer, BUFF_OUT_S, "$GPRMC,%02d%02d%02d,A,%02.0Lf%05.2Lf,%c,%03.0Lf%05.2Lf,%c,0,0,%02d%02d%02d,001.0,W*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, latstr, long_deg, long_min, longstr, utc->tm_mday, utc->tm_mon+1, utc->tm_year-100);
    csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, 64, "%02x\r\n", csum);
    nmea_str.append(buffer);
    return;
}

void AircraftProcessor::gpgga(std::string& nmea_str)
{
    nmea_str.clear();
    int csum;
    time_t now = time(0);
    tm* utc = gmtime(&now);
    snprintf(buffer, BUFF_OUT_S, "$GPGGA,%02d%02d%02d,%02.0Lf%06.4Lf,%c,%03.0Lf%07.4Lf,%c,1,05,1,%d,M,%.1f,M,,*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, latstr, long_deg, long_min, longstr, basealt, basegeoid);
    csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, 64, "%02x\r\n", csum);
    nmea_str.append(buffer);
    return;
}

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

AircraftProcessor::AircraftProcessor(double b_lat, double b_long, int b_alt, double geo)
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
    // calculate movement before relative position !
    calcMoveData(ac);
    calcRelPosToBase(ac);
    int poa = evaluatePOA(ac);

    if (dist > Configuration::filter_maxDist) {
        return "";
    }

    std::string nmea_str;
    Position& ac_pos = ac.getLastPosition();

    //PFLAU
    snprintf(buffer, BUFF_OUT_S, "$PFLAU,,,,1,%d,%d,0,%d,%d,%s*",
            poa, Math::dToI(bearing_rel), rel_V, ac_pos.distance, ac.id.c_str());
    int csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, LESS_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(buffer);
    //PFLAA
    if (ac.aircraft_type == MIN_DATA || ac.data_flags != FULL_DATA) {
        snprintf(buffer, BUFF_OUT_S, "$PFLAA,0,%d,%d,%d,1,%s,,,,,8*",
                rel_N, rel_E, rel_V, ac.id.c_str());
    } else {
        snprintf(buffer, BUFF_OUT_S, "$PFLAA,%d,%d,%d,%d,%u,%s,%03d,,%d,%3.1lf,%1x*",
                poa, rel_N, rel_E, rel_V, ac.id_type, ac.id.c_str(), Math::dToI(ac_pos.heading),
                Math::dToI(ac.gnd_speed * Math::ms2kmh), ac_pos.climb_rate, ac.aircraft_type);
    }
    csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, LESS_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(buffer);
    return nmea_str;
}

void AircraftProcessor::calcRelPosToBase(Aircraft& ac)
{
    Position& ac_pos = ac.getLastPosition();
    long_b = Math::radian(baselong);
    long_ac = Math::radian(ac_pos.longitude);
    lat_b = Math::radian(baselat);
    lat_ac = Math::radian(ac_pos.latitude);
    long_dist = long_ac - long_b;
    lat_dist = lat_ac -lat_b;

    a = std::pow(std::sin(lat_dist / 2.0), 2.0) + std::cos(lat_b) * std::cos(lat_ac) * std::pow(std::sin(long_dist / 2.0), 2.0);
    dist = Math::dToI(6371000.0 * (2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a))));
    ac_pos.distance = dist;

    bearing_rel = Math::degree(std::atan2(std::sin(long_ac - long_b) *
            std::cos(lat_ac), std::cos(lat_b) * std::sin(lat_ac) - std::sin(lat_b) *
            std::cos(lat_ac) * std::cos(long_ac - long_b)));
    bearing_abs = std::fmod((bearing_rel + 360.0), 360.0);

    rel_N = Math::dToI(std::cos(Math::radian(bearing_abs)) * dist);
    rel_E = Math::dToI(std::sin(Math::radian(bearing_abs)) * dist);
    rel_V = ac_pos.altitude - basealt;
    return;
}

void AircraftProcessor::calcMoveData(Aircraft& ac)
{
    if (!ac.isPosEvaluable()) return;
    Position& before = ac.getBeforeLastPosition();
    Position& last = ac.getLastPosition();

    double time_diff = last.timestamp - before.timestamp;
    if (time_diff <= 0.0 || time_diff > 99.0) return;

    long_b = Math::radian(before.longitude);
    long_ac = Math::radian(last.longitude);
    lat_b = Math::radian(before.latitude);
    lat_ac = Math::radian(last.latitude);
    long_dist = long_ac - long_b;
    lat_dist = lat_ac -lat_b;
    a = std::pow(std::sin(lat_dist / 2.0), 2.0) + std::cos(lat_b) * std::cos(lat_ac) * std::pow(std::sin(long_dist / 2.0), 2.0);
    dist = Math::dToI(6371000.0 * (2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a))));

    if ((ac.data_flags & SPEED_FLAG) == 0) {
        //calculate ground speed
        ac.gnd_speed = dist / time_diff;
        ac.data_flags |= SPEED_FLAG;
    }
    if ((ac.data_flags & CLIMB_FLAG) == 0) {
        //calculate climb rate
        last.climb_rate = (last.altitude - before.altitude) / time_diff;
        ac.data_flags |= CLIMB_FLAG;
    }
    if ((ac.data_flags & HEADING_FLAG) == 0) {
        //calculate heading
        last.heading = std::fmod(((Math::degree(
                std::atan2(
                        std::sin(long_ac - long_b) * std::cos(lat_ac),
                        std::cos(lat_b) * std::sin(lat_ac) - std::sin(lat_b) *
                        std::cos(lat_ac) * std::cos(long_ac - long_b))
        )) + 360.0), 360.0);
        ac.data_flags |= HEADING_FLAG;
    }
    if ((ac.data_flags & TURN_FLAG) == 0) {
        //calculate turn rate
        if (before.heading != VALUE_NA) {
            int head_l = (last.heading == 0) ? 360 : last.heading;
            int head_b = (before.heading == 0) ? 360 : before.heading;
            last.turn_rate = (head_l - head_b) / time_diff;
            ac.data_flags |= TURN_FLAG;
        } else {
            last.turn_rate = VALUE_NA;
        }
    }
    if (ac.aircraft_type == MIN_DATA) ac.aircraft_type = ADSB_T;
    return;
}

void AircraftProcessor::calcPOA(Aircraft& ac)
{
    return;
}

int AircraftProcessor::evaluatePOA(Aircraft& ac)
{
    //if there are not enough informations available, no calculation must be done
    if (ac.data_flags != FULL_DATA) return 0;

    calcPOA(ac);
    int level = 0;
    Position& last = ac.getLastPosition();

    //check wether ac is in range of 5 mins
    if (last.distance / ac.gnd_speed <= 300.0) return 0;

    //check wether interpolated heading is in direction of base
    //check wether interpolated glidepath is in range of base and height is sufficient

    return level;
}

void AircraftProcessor::gpsfix(std::string& nmea_str)
{
    nmea_str.clear();
    int csum;

    latstr = (baselat < 0)? 'S' : 'N';
    longstr = (baselong < 0)? 'W' : 'E';
    lat_deg = std::abs(std::floor(baselat));
    lat_min = std::abs(60.0 * (baselat - lat_deg));
    long_deg = std::abs(std::floor(baselong));
    long_min = std::abs(60.0 * (baselong - long_deg));

    time_t now = time(0);
    tm* utc = gmtime(&now);
    //gprmc
    snprintf(buffer, BUFF_OUT_S, "$GPRMC,%02d%02d%02d,A,%02.0lf%05.2lf,%c,%03.0lf%05.2lf,%c,0,0,%02d%02d%02d,001.0,W*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, latstr, long_deg, long_min, longstr, utc->tm_mday, utc->tm_mon+1, utc->tm_year-100);
    csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, 64, "%02x\r\n", csum);
    nmea_str.append(buffer);
    //gpgga
    snprintf(buffer, BUFF_OUT_S, "$GPGGA,%02d%02d%02d,%02.0lf%06.4lf,%c,%03.0lf%07.4lf,%c,1,05,1,%d,M,%.1lf,M,,*",
            utc->tm_hour, utc->tm_min, utc->tm_sec, lat_deg, lat_min, latstr, long_deg, long_min, longstr, basealt, basegeoid);
    csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, 64, "%02x\r\n", csum);
    nmea_str.append(buffer);
    return;
}

/*
 * Parser.cpp
 *
 *  Created on: 17.04.2016
 *      Author: lula
 */

#include "Parser.h"
#include <cmath>

Parser::Parser(long double b_lat, long double b_long, int b_alt)
: PI(std::acos(-1.0L)),
  baselat(b_lat),
  baselong(b_long),
  basealt(b_alt)
{
}

Parser::~Parser()
{
}

int Parser::checksum(const char* sentence) const
{
    int csum = 0;
    int i=1;
    while (sentence[i] != '*') {
        csum ^= (int) sentence[i++];
    }
    return csum;
}

long double Parser::radian(long double deg) const
{
    return ((deg * PI) / 180.0L);
}

long double Parser::degree(long double rad) const
{
    return (rad * 180.0L) / PI;
}

int Parser::ldToI(long double ld) const
{
    return (int)(ld + 0.5L);
}

void Parser::calcPosInfo(Aircraft* ac)
{
    long_b = radian(baselong);
    long_ac = radian((*ac).longitude);
    lat_b = radian(baselat);
    lat_ac = radian((*ac).latitude);
    //printf("\nlon1= %.17Lf, lat1= %.17Lf, lon2= %.17Lf, lat2= %.17Lf\n", long_b,lat_b,long_ac,lat_ac);
    long_dist = long_ac - long_b;
    lat_dist = lat_ac -lat_b;

    a = std::pow(std::sin(lat_dist/2.0L),2.0L) + std::cos(lat_b) * std::cos(lat_ac) * std::pow(std::sin(long_dist/2.0L),2.0L);
    //printf("a= %.17Lf\n", a);
    c = 2.0L * std::atan2(std::sqrt(a), std::sqrt(1.0L - a));
    //printf("c= %.17Lf\n",c);
    dist = 6371000.0L * c;
    //printf("distance= %.17Lf\n", dist);

    bearing = std::atan2(std::sin(long_ac-long_b)*std::cos(lat_ac), std::cos(lat_b)*std::sin(lat_ac)-std::sin(lat_b)*std::cos(lat_ac)*std::cos(long_ac-long_b));
    //printf("bearing= %.17Lf\n",bearing);
    bearing_rel = degree(bearing);
    //printf("relBearing= %.17Lf\n",bearing_rel);
    bearing_abs = std::fmod((bearing_rel + 360.0L), 360.0L);
    //printf("absBearing= %.17Lf\n",bearing_abs);

    rel_N = std::cos(radian(bearing_abs)) * dist;
    //printf("relNorth= %.17Lf, rad(absBearing)= %.17Lf\n",rel_N, radian(bearing_abs));
    rel_E = std::sin(radian(bearing_abs)) * dist;
    //printf("relEast= %.17Lf\n",rel_E);
    rel_V = (*ac).altitude / 3.2808L - basealt;

    return;
}

void Parser::gprmc(std::string& nmea_str) {
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
}

/*
 * ParserOGN.cpp
 *
 *  Created on: 17.04.2016
 *      Author: lula
 */

#include "ParserOGN.h"
#include <cmath>
#include <cstdio>
#include <ctime>

ParserOGN::ParserOGN()
{
}

ParserOGN::~ParserOGN()
{
}

int ParserOGN::unpack(Aircraft& extAc, const std::string& sentence) const
{
   //hard stuff to do
   return 0;
}

void ParserOGN::process(Aircraft& ac, std::string& nmea_str, long double baselat,
      long double baselong, int basealt)
{
  // Aircraft ac_tmp = ac;
   ExtendedAircraft extAc = (ExtendedAircraft&) ac;//_tmp;
   calcPosInfo(extAc, baselat, baselong, basealt);
   nmea_str.clear();

   //PFLAU
   snprintf(buffer, BUFF_OUT_S, "$PFLAU,,,,1,0,%d,0,%d,%u,%s*", ldToI(bearing_rel),
         ldToI(rel_V), ldToI(dist), extAc.address.c_str());
   int csum = checksum(buffer);
   nmea_str.append(buffer);
   snprintf(buffer, 64, "%02x\r\n", csum);
   nmea_str.append(buffer);
   //PFLAA
   //$PFLAA,0,%d,%d,%d,%s,%s,%03.0f,,%s,%s,%s* ::: has to be modified when types are fixed
   snprintf(buffer, BUFF_OUT_S, "$PFLAA,0,%d,%d,%d,%d,%s,%03.0f,,%d,%d,%d*", ldToI(rel_N),
         ldToI(rel_E), ldToI(rel_V), extAc.address_type, extAc.address.c_str(), extAc.track, extAc.ground_speed, extAc.climb_rate, extAc.aircraft_type);
   csum = checksum(buffer);
   nmea_str.append(buffer);
   snprintf(buffer, 64, "%02x\r\n", csum);
   nmea_str.append(buffer);
   //GPRMC
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

long double ParserOGN::dmsToDeg(long double dms) const
{
   long double absDms = std::abs(dms);
   long double d = std::floor(absDms);
   long double m = (absDms - d) * 100.0L / 60.0L;
   return d + m;
}

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

ParserOGN::ParserOGN(long double b_lat, long double b_long, int b_alt)
: Parser(b_lat, b_long, b_alt)
{
}

ParserOGN::~ParserOGN()
{
}

int ParserOGN::unpack(Aircraft& extAc, const std::string& sentence) const
{
    if (sentence.at(0) == '#') {
        return 1;
    }
    //hard stuff to do
    return 0;
}

void ParserOGN::process(Aircraft& ac, std::string& nmea_str)
{
    // Aircraft ac_tmp = ac;
    ExtendedAircraft extAc = (ExtendedAircraft&) ac;//_tmp;
    calcPosInfo(extAc);
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
    return;
}

long double ParserOGN::dmsToDeg(long double dms) const
{
    long double absDms = std::abs(dms);
    long double d = std::floor(absDms);
    long double m = (absDms - d) * 100.0L / 60.0L;
    return d + m;
}

/*
 * ParserOGN.cpp
 *
 *  Created on: 17.04.2016
 *      Author: lula
 */

#include "ParserOGN.h"
#include "ExtendedAircraft.h"
#include <cmath>
#include <cstdio>
#include <ctime>
#include <regex>
#include <sstream>

ParserOGN::ParserOGN(long double b_lat, long double b_long, int b_alt)
: Parser(b_lat, b_long, b_alt)
{
}

ParserOGN::~ParserOGN()
{
}

int ParserOGN::unpack(Aircraft& ac, const std::string& sentence)
{
    if (sentence.at(0) == '#') {
        return -1;
    }
    ExtendedAircraft& extAc = dynamic_cast<ExtendedAircraft&>(ac);

    try {
        std::regex aprs_re("^(.+?)>APRS,.+,(.+?):/(\d{6})+h(\d{4}\.\d{2})(N|S)(.)(\d{5}\.\d{2})(E|W)(.)((\d{3})/(\d{3}))?/A=(\d{6})\s(.*)$");
        std::smatch match;
        if (std::regex_search(sentence, match, aprs_re) && match.size() > 1) {
            //latitude
            extAc.latitude = dmsToDeg(std::stold(match.str(4)) / 100.0L);
            if (match.str(5).compare("S") == 0) extAc.latitude = -extAc.latitude;
            //longitue
            extAc.longitude = dmsToDeg(std::stold(match.str(7)) / 100.0L);
            if (match.str(8).compare("W") == 0) extAc.longitude = -extAc.longitude;
            //track/gnd_speed
            if (match.str(10).size() > 0) {
                extAc.track = std::stoi(match.str(11));
                extAc.ground_speed = ldToI(std::stold(match.str(12)) * kts2kmh);
            } else {
                extAc.track = 0;
                extAc.ground_speed = 0;
            }
            //altitude
            if (match.str(13).size() > 0) extAc.altitude = ldToI(std::stold(match.str(13)) * feet2m);
            //comment
            if (match.str(14).size() > 0) extAc.comment = match.str(14);

            //climbrate / address
            std::regex addr_re("id(\S{2})(\S{6})");
            std::regex climb_re("([\+\-]\d+)fpm");
            splitToTokens(extAc.comment);
            for (std::string part : tokens) {
                std::smatch addr_match;
                std::smatch climb_match;
                if (std::regex_match(part, addr_match, addr_re)) {
                    extAc.address_type = std::stoi(addr_match.str(1), nullptr, 16) & 0x03;
                    extAc.aircraft_type = (std::stoi(addr_match.str(1), nullptr, 16) & 0x7C) >> 2;
                    extAc.address = addr_match.str(2);
                } else if (std::regex_match(part, climb_match, climb_re)) {
                    extAc.climb_rate = ldToI(std::stold(climb_match.str(1)) * fpm2ms);
                }
            }
        }
    } catch (std::regex_error& e) {
        printf("%s\n", e.what());
    }
    return 0;
}

void ParserOGN::process(Aircraft& ac, std::string& nmea_str)
{

    ExtendedAircraft extAc = (ExtendedAircraft&) ac;
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

void ParserOGN::splitToTokens(const std::string& str) {
    tokens.clear();
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, ' ')) {
        tokens.push_back(item);
    }
    return;
}

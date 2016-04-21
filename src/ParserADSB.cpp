#include <cmath>
#include <cstdio>
#include <ctime>
#include "ParserADSB.h"

ParserADSB::ParserADSB(long double b_lat, long double b_long, int b_alt)
: Parser(b_lat, b_long, b_alt)
{
}

ParserADSB::~ParserADSB()
{
}

int ParserADSB::unpack(Aircraft& ac, const std::string& sentence)
{
    std::string msg = sentence;
    /*
     * fields:
     * 4 : id
     * 11: altitude
     * 14: latitude
     * 15: longitude
     */
    msg.erase(0,6);
    int delim, index = 2;
    //last param in msg is unprocessed
    while((delim = msg.find(',')) > -1) {
        switch(index) {
        case 4:
            ac.id = msg.substr(0,delim);
            break;
        case 11:
            if (msg.substr(0,delim).length() > 0)
                ac.altitude = std::stoi(msg.substr(0,delim), nullptr);
            else return -1;
            break;
        case 14:
            if (msg.substr(0,delim).length() > 0)
                ac.latitude = std::stold(msg.substr(0,delim), nullptr);
            else return -1;
            break;
        case 15:
            if (msg.substr(0,delim).length() > 0)
                ac.longitude = std::stold(msg.substr(0,delim), nullptr);
            else return -1;
            break;
        default:
            break;
        }
        index++;
        msg.erase(0,delim+1);
    }
    return 0;
}

void ParserADSB::process(Aircraft& ac, std::string& nmea_str)
{
    calcPosInfo(ac);
    nmea_str.clear();

    //PFLAU
    snprintf(buffer, BUFF_OUT_S, "$PFLAU,,,,1,0,%d,0,%d,%u,%s*", ldToI(bearing_rel),
            ldToI(rel_V), ldToI(dist), ac.id.c_str());
    int csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, 64, "%02x\r\n", csum);
    nmea_str.append(buffer);
    //PFLAA
    snprintf(buffer, BUFF_OUT_S, "$PFLAA,0,%d,%d,%d,1,%s,,,,,8*", ldToI(rel_N),
            ldToI(rel_E), ldToI(rel_V), ac.id.c_str());
    csum = checksum(buffer);
    nmea_str.append(buffer);
    snprintf(buffer, 64, "%02x\r\n", csum);
    nmea_str.append(buffer);
    return;
}

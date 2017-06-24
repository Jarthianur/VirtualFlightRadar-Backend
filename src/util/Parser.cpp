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

#include "Parser.h"

#include <boost/regex.hpp>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include "../aircraft/Aircraft.h"
#include "../config/Configuration.h"
#include "Math.hpp"
#include "Position.hpp"

//#define MATCH_TIME 1
#define RE_APRS_LAT      1
#define RE_APRS_LAT_DIR  2
#define RE_APRS_LONG     3
#define RE_APRS_LONG_DIR 4
#define RE_APRS_HEAD     5
#define RE_APRS_GND_SPD  6
#define RE_APRS_ALT      7
#define RE_APRS_COM      8
#define RE_APRS_COM_TYPE 1
#define RE_APRS_COM_ID   2
#define RE_APRS_COM_CR   3
#define RE_APRS_COM_TR   4
#define RE_GGA_LAT       1
#define RE_GGA_LAT_DIR   2
#define RE_GGA_LONG      3
#define RE_GGA_LONG_DIR  4
#define RE_GGA_FIX       5
#define RE_GGA_SAT       6
#define RE_GGA_DIL       7
#define RE_GGA_ALT       8
#define RE_GGA_GEOID     9

namespace util
{

const boost::regex Parser::aprsRe(
        //"^(?:\\S+?)>APRS,\\S+?(?:,\\S+?)?:/(\\d{6})h(\\d{4}\\.\\d{2})([NS])[\\S\\s]+?(\\d{5}\\.\\d{2})([EW])[\\S\\s]+?(?:(\\d{3})/(\\d{3}))?/A=(\\d{6})\\s+?([\\S\\s]+?)$",
        "^(?:\\S+?)>APRS,\\S+?(?:,\\S+?)?:/\\d{6}h(\\d{4}\\.\\d{2})([NS])[\\S\\s]+?(\\d{5}\\.\\d{2})([EW])[\\S\\s]+?(?:(\\d{3})/(\\d{3}))?/A=(\\d{6})\\s+?([\\S\\s]+?)$",
        boost::regex::optimize | boost::regex::icase);

const boost::regex Parser::aprsComRe(
        "^(?:[\\S\\s]+?)?id([0-9A-F]{2})([0-9A-F]{6})\\s?(?:([\\+-]\\d{3})fpm\\s+?)?(?:([\\+-]\\d+?\\.\\d+?)rot)?(?:[\\S\\s]+?)?$",
        boost::regex::optimize | boost::regex::icase);

const boost::regex Parser::gpggaRe(
        "^\\$[A-Z]{2}GGA,\\d{6},(\\d{4}\\.\\d{3,4}),([NS]),(\\d{5}\\.\\d{3,4}),([EW]),(\\d),(\\d{2}),(\\d+(?:\\.\\d+)?),(\\d+(?:\\.\\d+)?),M,(\\d+(?:\\.\\d+)?),M,,\\*[0-9A-F]{2}\\s+?$",
        boost::regex::optimize | boost::regex::icase);

Parser::Parser()
{
}

Parser::~Parser() noexcept
{
}

aircraft::Aircraft Parser::parseAprs(const std::string& cr_msg)
{
    if (cr_msg.size() > 0 && cr_msg.at(0) == '#')
    {
        throw std::logic_error("");
    }
    bool fullInfo = true;
    std::int32_t idType = 0, acType = 0;
    //std::int_32_t time = 0;
    double turnRate = 0.0, climbRate = 0.0, gndSpeed = 0.0, heading = 0.0;
    GpsPosition gpsPos;
    std::string id;
    boost::smatch match;
    if (boost::regex_match(cr_msg, match, aprsRe))
    {
        //time
        //time = std::stoi(match.str(1));
        //latitude
        gpsPos.latitude = math::dmToDeg(std::stod(match.str(RE_APRS_LAT)));
        if (match.str(RE_APRS_LAT_DIR).compare("S") == 0)
        {
            gpsPos.latitude = -gpsPos.latitude;
        }
        //longitude
        gpsPos.longitude = math::dmToDeg(std::stod(match.str(RE_APRS_LONG)));
        if (match.str(RE_APRS_LONG_DIR).compare("W") == 0)
        {
            gpsPos.longitude = -gpsPos.longitude;
        }
        //altitude
        gpsPos.altitude = math::dToI(std::stod(match.str(RE_APRS_ALT)) * math::FEET_2_M);
        if (gpsPos.altitude > config::Configuration::filter_maxHeight)
        {
            throw std::logic_error("");
        }
        //comment
        // climbrate / address / id / type
        if (match.str(RE_APRS_COM).size() > 0)
        {
            std::string comm = match.str(RE_APRS_COM); // regex bug ! cannot work inplace, need to copy submatch.
            boost::smatch com_match;
            if (boost::regex_match(comm, com_match, aprsComRe))
            {
                id = com_match.str(RE_APRS_COM_ID);
                idType = std::stoi(com_match.str(RE_APRS_COM_TYPE), nullptr, 16) & 0x03;
                acType = (std::stoi(com_match.str(RE_APRS_COM_TYPE), nullptr, 16) & 0x7C)
                        >> 2;
                try
                {
                    climbRate = std::stod(com_match.str(RE_APRS_COM_CR)) * math::FPM_2_MS;
                } catch (const std::logic_error& e)
                {
                    climbRate = A_VALUE_NA;
                    fullInfo = false;
                }
                try
                {
                    turnRate = std::stod(com_match.str(RE_APRS_COM_TR)) * 3.0; // 1rot = 1 halfcircle / 1 min => 3° / 1s
                } catch (const std::logic_error& e)
                {
                    turnRate = A_VALUE_NA;
                    fullInfo = false;
                }
            } else
            {
                throw std::logic_error("");
            }
        } else
        {
            throw std::logic_error("");
        }
        //track/gnd_speed
        try
        {
            heading = std::stod(match.str(RE_APRS_HEAD));
        } catch (const std::logic_error& e)
        {
            heading = A_VALUE_NA;
            fullInfo = false;
        }
        try
        {
            gndSpeed = std::stod(match.str(RE_APRS_GND_SPD)) * math::KTS_2_MS;
        } catch (const std::logic_error& e)
        {
            gndSpeed = A_VALUE_NA;
            fullInfo = false;
        }
    } else
    {
        throw std::logic_error("");
    }
    aircraft::Aircraft ac(id, gpsPos, gndSpeed, idType, acType, climbRate, turnRate,
            heading);
    ac.setFullInfo(fullInfo);
    ac.setTargetT(aircraft::Aircraft::TargetType::FLARM);
    return ac;
}

aircraft::Aircraft Parser::parseSbs(const std::string& cr_msg)
{
    /*
     * fields:
     * 4 : id
     * 7 : time
     * 11: altitude
     * 14: latitude
     * 15: longitude
     */
    std::size_t p = 6;
    if (cr_msg.find(',', p) == std::string::npos
            || !(cr_msg.size() > 4 && cr_msg.at(4) == '3'))
    {
        throw std::logic_error("");
    }
    std::size_t delim;
    std::uint32_t i = 2;
    std::string id;
    GpsPosition gpsPos;
    while ((delim = cr_msg.find(',', p)) != std::string::npos && i < 16)
    {
        switch (i)
        {
            case 4:
                if (delim - p > 0)
                {
                    id = cr_msg.substr(p, delim - p);
                } else
                {
                    throw std::logic_error("");
                }
                break;
                /*case 7:
                 try
                 {
                 if (delim - p > 7)
                 {
                 mtTime = std::stoi(cr_msg.substr(p, 2)) * 10000;
                 mtTime += std::stoi(cr_msg.substr(p + 3, 2)) * 100;
                 mtTime += std::stoi(cr_msg.substr(p + 6, 2));
                 } else
                 {
                 return MSG_UNPACK_IGN;
                 }
                 } catch (const std::logic_error& e)
                 {
                 return MSG_UNPACK_ERR;
                 }
                 break;*/
            case 11:
                gpsPos.altitude = math::dToI(
                        std::stod(cr_msg.substr(p, delim - p)) * math::FEET_2_M);
                if (gpsPos.altitude > config::Configuration::filter_maxHeight)
                {
                    throw std::logic_error("");
                }
                break;
            case 14:
                gpsPos.latitude = std::stod(cr_msg.substr(p, delim - p));
                break;
            case 15:
                gpsPos.longitude = std::stod(cr_msg.substr(p, delim - p));
                break;
            default:
                break;
        }
        i++;
        p = delim + 1;
    }
    aircraft::Aircraft ac(id, gpsPos);
    ac.setFullInfo(false);
    ac.setTargetT(aircraft::Aircraft::TargetType::TRANSPONDER);
    return ac;
}

ExtGpsPosition Parser::parseGpsNmea(const std::string& cr_msg)
{
    std::int32_t csum = std::stoi(cr_msg.substr(cr_msg.rfind('*') + 1, 2), nullptr, 16);
    if (csum != math::checksum(cr_msg.c_str(), cr_msg.length()))
    {
        throw std::logic_error("");
    }
    ExtGpsPosition gpsPos;
    boost::smatch match;
    if (boost::regex_match(cr_msg, match, gpggaRe))
    {
        //latitude
        gpsPos.position.latitude = math::dmToDeg(std::stod(match.str(1)));
        if (match.str(2).compare("S") == 0)
        {
            gpsPos.position.latitude = -gpsPos.position.latitude;
        }
        //longitude
        gpsPos.position.longitude = math::dmToDeg(std::stod(match.str(3)));
        if (match.str(4).compare("W") == 0)
        {
            gpsPos.position.longitude = -gpsPos.position.longitude;
        }
        //fix
        gpsPos.fixQa = std::stoi(match.str(5));
        //sats
        gpsPos.nrSats = std::stoi(match.str(6));
        //dilution
        gpsPos.dilution = std::stod(match.str(7));
        //altitude
        gpsPos.position.altitude = math::dToI(std::stod(match.str(8)));
        //geoid
        gpsPos.geoid = std::stod(match.str(9));
    } else
    {
        throw std::logic_error("");
    }
    return gpsPos;
}

SensorInfo Parser::parseSensNmea(const std::string& cr_msg)
{
    std::int32_t csum = std::stoi(cr_msg.substr(cr_msg.rfind('*') + 1, 2), nullptr, 16);
    if (csum != math::checksum(cr_msg.c_str(), cr_msg.length()))
    {
        throw std::logic_error("");
    }
    SensorInfo info;
    if (cr_msg.find("MDA") != std::string::npos)
    {
        info.mdaStr = cr_msg;
        std::size_t tmpB = cr_msg.find('B') - 1;
        std::size_t tmpS = cr_msg.substr(0, tmpB).find_last_of(',') + 1;
        std::size_t subLen = tmpB - tmpS;
        std::size_t numIdx;
        double tmpPress = std::stod(cr_msg.substr(tmpS, subLen), &numIdx) * 1000.0;
        if (numIdx == subLen)
        {
            info.press = tmpPress;
        } else
        {
            throw std::logic_error("");
        }
    } else if (cr_msg.find("MWV") != std::string::npos)
    {
        info.mwvStr = cr_msg;
    } else
    {
        throw std::logic_error("");
    }
    return info;
}

}  // namespace util

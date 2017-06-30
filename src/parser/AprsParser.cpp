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

#include "AprsParser.h"

#include <boost/regex.hpp>
#include <stdexcept>

#include "../config/Configuration.h"
#include "../util/Math.hpp"

/// Define regex match groups for APRS
//#define RE_APRS_TIME 1
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

namespace parser
{

const boost::regex AprsParser::msAprsRe(
        //"^(?:\\S+?)>APRS,\\S+?(?:,\\S+?)?:/(\\d{6})h(\\d{4}\\.\\d{2})([NS])[\\S\\s]+?(\\d{5}\\.\\d{2})([EW])[\\S\\s]+?(?:(\\d{3})/(\\d{3}))?/A=(\\d{6})\\s+?([\\S\\s]+?)$",
        "^(?:\\S+?)>APRS,\\S+?(?:,\\S+?)?:/\\d{6}h(\\d{4}\\.\\d{2})([NS])[\\S\\s]+?(\\d{5}\\.\\d{2})([EW])[\\S\\s]+?(?:(\\d{3})/(\\d{3}))?/A=(\\d{6})\\s+?([\\S\\s]+?)$",
        boost::regex::optimize | boost::regex::icase);

const boost::regex AprsParser::msAprsComRe(
        "^(?:[\\S\\s]+?)?id([0-9A-F]{2})([0-9A-F]{6})\\s?(?:([\\+-]\\d{3})fpm\\s+?)?(?:([\\+-]\\d+?\\.\\d+?)rot)?(?:[\\S\\s]+?)?$",
        boost::regex::optimize | boost::regex::icase);

AprsParser::AprsParser()
        : Parser()
{
}

AprsParser::~AprsParser() noexcept
{
}

bool AprsParser::unpack(const std::string& cr_msg, aircraft::Aircraft& r_ac) noexcept
{
    if (cr_msg.size() > 0 && cr_msg.at(0) == '#')
    {
        return false;
    }
    bool fullInfo = true;
    struct util::GpsPosition pos;
    boost::smatch match;
    if (boost::regex_match(cr_msg, match, msAprsRe))
    {
        try
        {
            //time
            //time = std::stoi(match.str(1));

            //latitude
            pos.latitude = util::math::dmToDeg(std::stod(match.str(RE_APRS_LAT)));
            if (match.str(RE_APRS_LAT_DIR).compare("S") == 0)
            {
                pos.latitude = -pos.latitude;
            }

            //longitude
            pos.longitude = util::math::dmToDeg(std::stod(match.str(RE_APRS_LONG)));
            if (match.str(RE_APRS_LONG_DIR).compare("W") == 0)
            {
                pos.longitude = -pos.longitude;
            }

            //altitude
            pos.altitude = util::math::dToI(
                    std::stod(match.str(RE_APRS_ALT)) * util::math::FEET_2_M);
            if (pos.altitude > config::Configuration::filter_maxHeight)
            {
                return false;
            }
        } catch (const std::logic_error& e)
        {
            return false;
        }
        //comment
        // climbrate / address / id / type
        if (match.str(RE_APRS_COM).size() > 0)
        {
            std::string comm = match.str(RE_APRS_COM); // regex bug ! cannot work inplace, need to copy submatch.
            boost::smatch com_match;
            if (boost::regex_match(comm, com_match, msAprsComRe))
            {
                r_ac.setId(com_match.str(RE_APRS_COM_ID));
                try
                {
                    r_ac.setIdType(
                            std::stoi(com_match.str(RE_APRS_COM_TYPE), nullptr, 16)
                                    & 0x03);
                    r_ac.setAircraftT(
                            (std::stoi(com_match.str(RE_APRS_COM_TYPE), nullptr, 16)
                                    & 0x7C) >> 2);
                } catch (const std::logic_error& e)
                {
                    return false;
                }
                try
                {
                    r_ac.setClimbRate(
                            std::stod(com_match.str(RE_APRS_COM_CR))
                                    * util::math::FPM_2_MS);
                } catch (const std::logic_error& e)
                {
                    r_ac.setClimbRate();
                    fullInfo = false;
                }
                /*try
                 {
                 turnRate = std::stod(com_match.str(RE_APRS_COM_TR)) * 3.0; // 1rot = 1 halfcircle / 1 min => 3° / 1s
                 } catch (const std::logic_error& e)
                 {
                 turnRate = A_VALUE_NA;
                 fullInfo = false;
                 }*/
            } else
            {
                return false;
            }
        } else
        {
            return false;
        }

        //track/gnd_speed
        try
        {
            r_ac.setHeading(std::stod(match.str(RE_APRS_HEAD)));
        } catch (const std::logic_error& e)
        {
            r_ac.setHeading();
            fullInfo = false;
        }
        try
        {
            r_ac.setGndSpeed(
                    std::stod(match.str(RE_APRS_GND_SPD)) * util::math::KTS_2_MS);
        } catch (const std::logic_error& e)
        {
            r_ac.setGndSpeed();
            fullInfo = false;
        }
        r_ac.setPosition(pos);
        r_ac.setFullInfo(fullInfo);
        r_ac.setTargetT(aircraft::Aircraft::TargetType::FLARM);
    } else
    {
        return false;
    }
    return true;
}

}  // namespace parser
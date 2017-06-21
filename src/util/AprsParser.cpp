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
#include <algorithm>
#include <stdexcept>

#include "../aircraft/Aircraft.h"
#include "../config/Configuration.h"
#include "../data/AircraftContainer.h"
#include "../util/Logger.h"
#include "../util/Math.hpp"
#include "../VFRB.h"

//#define MATCH_TIME 1
#define MATCH_LAT      1
#define MATCH_LAT_DIR  2
#define MATCH_LONG     3
#define MATCH_LONG_DIR 4
#define MATCH_HEAD     5
#define MATCH_GND_SPD  6
#define MATCH_ALT      7
#define MATCH_COM      8
#define MATCH_COM_TYPE 1
#define MATCH_COM_ID   2
#define MATCH_COM_CR   3
#define MATCH_COM_TR   4

namespace parser
{

AprsParser::AprsParser()
        : Parser(),
          mAprsRe(
                  //"^(?:\\S+?)>APRS,\\S+?(?:,\\S+?)?:/(\\d{6})h(\\d{4}\\.\\d{2})([NS])[\\S\\s]+?(\\d{5}\\.\\d{2})([EW])[\\S\\s]+?(?:(\\d{3})/(\\d{3}))?/A=(\\d{6})\\s+?([\\S\\s]+?)$",
                  "^(?:\\S+?)>APRS,\\S+?(?:,\\S+?)?:/\\d{6}h(\\d{4}\\.\\d{2})([NS])[\\S\\s]+?(\\d{5}\\.\\d{2})([EW])[\\S\\s]+?(?:(\\d{3})/(\\d{3}))?/A=(\\d{6})\\s+?([\\S\\s]+?)$",
                  boost::regex::optimize | boost::regex::icase),
          mComRe(
                  "^(?:[\\S\\s]+?)?id([0-9A-F]{2})([0-9A-F]{6})\\s?(?:([\\+-]\\d{3})fpm\\s+?)?(?:([\\+-]\\d+?\\.\\d+?)rot)?(?:[\\S\\s]+?)?$",
                  boost::regex::optimize | boost::regex::icase)
{
}

AprsParser::~AprsParser() noexcept
{
}

std::int32_t AprsParser::unpack(const std::string& cr_msg, std::int32_t prio)
noexcept
{
    if (cr_msg.size() > 0 && cr_msg.at(0) == '#')
    {
        return MSG_UNPACK_IGN;
    }
    bool fullInfo = true;
    std::int32_t idType = 0, acType = 0;
    //std::int_32_t time = 0;
    double turnRate = 0.0, climbRate = 0.0, gndSpeed = 0.0, heading = 0.0;
    boost::smatch match;
    if (boost::regex_match(cr_msg, match, mAprsRe))
    {
        try
        {
            //time
            //time = std::stoi(match.str(1));

            //latitude
            mtGpsPos.latitude = util::math::dmToDeg(std::stod(match.str(MATCH_LAT)));
            if (match.str(MATCH_LAT_DIR).compare("S") == 0)
            {
                mtGpsPos.latitude = -mtGpsPos.latitude;
            }

            //longitude
            mtGpsPos.longitude = util::math::dmToDeg(std::stod(match.str(MATCH_LONG)));
            if (match.str(MATCH_LONG_DIR).compare("W") == 0)
            {
                mtGpsPos.longitude = -mtGpsPos.longitude;
            }

            //altitude
            mtGpsPos.altitude = util::math::dToI(
                    std::stod(match.str(MATCH_ALT)) * util::math::FEET_2_M);
            if (mtGpsPos.altitude > config::Configuration::filter_maxHeight)
            {
                return MSG_UNPACK_IGN;
            }
        } catch (const std::logic_error& e)
        {
            return MSG_UNPACK_ERR;
        }
        //comment
        // climbrate / address / id / type
        if (match.str(MATCH_COM).size() > 0)
        {
            std::string comm = match.str(MATCH_COM); // regex bug ! cannot work inplace, need to copy submatch.
            boost::smatch com_match;
            if (boost::regex_match(comm, com_match, mComRe))
            {
                mtId = com_match.str(MATCH_COM_ID);
                try
                {
                    idType = std::stoi(com_match.str(MATCH_COM_TYPE), nullptr, 16) & 0x03;
                    acType =
                            (std::stoi(com_match.str(MATCH_COM_TYPE), nullptr, 16) & 0x7C)
                                    >> 2;
                } catch (const std::logic_error& e)
                {
                    return MSG_UNPACK_ERR;
                }
                try
                {
                    climbRate = std::stod(com_match.str(MATCH_COM_CR))
                            * util::math::FPM_2_MS;
                } catch (const std::logic_error& e)
                {
                    climbRate = A_VALUE_NA;
                    fullInfo = false;
                }
                try
                {
                    turnRate = std::stod(com_match.str(MATCH_COM_TR)) * 3.0; // 1rot = 1 halfcircle / 1 min => 3° / 1s
                } catch (const std::logic_error& e)
                {
                    turnRate = A_VALUE_NA;
                    fullInfo = false;
                }
            } else
            {
                return MSG_UNPACK_IGN;
            }
        } else
        {
            return MSG_UNPACK_IGN;
        }

        //track/gnd_speed
        try
        {
            heading = std::stod(match.str(MATCH_HEAD));
        } catch (const std::logic_error& e)
        {
            heading = A_VALUE_NA;
            fullInfo = false;
        }
        try
        {
            gndSpeed = std::stod(match.str(MATCH_GND_SPD)) * util::math::KTS_2_MS;
        } catch (const std::logic_error& e)
        {
            gndSpeed = A_VALUE_NA;
            fullInfo = false;
        }
        aircraft::Aircraft ac(mtId, mtGpsPos, gndSpeed, idType, acType, climbRate,
                turnRate, heading);
        ac.setFullInfo(fullInfo);
        ac.setTargetT(aircraft::Aircraft::TargetType::FLARM);
        VFRB::msAcCont.insertAircraft(ac, prio);
    } else
    {
        return MSG_UNPACK_IGN;
    }
    return MSG_UNPACK_SUC;
}

}  // namespace parser

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
#include "../vfrb/VFRB.h"

namespace parser
{

AprsParser::AprsParser()
        : Parser(),
          mAprsRe(
                  "^(?:\\S+?)>APRS,\\S+?(?:,\\S+?)?:/(\\d{6})h(\\d{4}\\.\\d{2})([NS])[\\S\\s]+?(\\d{5}\\.\\d{2})([EW])[\\S\\s]+?(?:(\\d{3})/(\\d{3}))?/A=(\\d{6})\\s+?([\\S\\s]+?)$",
                  boost::regex::optimize | boost::regex::icase),
          mCommRe(
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
    bool tmpFullInfo = true;
    boost::smatch match;
    if (boost::regex_match(cr_msg, match, mAprsRe))
    {
        try
        {
            //time
            mtTime = std::stoi(match.str(1));

            //latitude
            mtGpsPos.latitude = util::math::dmToDeg(std::stod(match.str(2)));
            if (match.str(3).compare("S") == 0)
            {
                mtGpsPos.latitude = -mtGpsPos.latitude;
            }

            //longitude
            mtGpsPos.longitude = util::math::dmToDeg(std::stod(match.str(4)));
            if (match.str(5).compare("W") == 0)
            {
                mtGpsPos.longitude = -mtGpsPos.longitude;
            }

            //altitude
            mtGpsPos.altitude = util::math::dToI(
                    std::stod(match.str(8)) * util::math::FEET_2_M);
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
        if (match.str(9).size() > 0)
        {
            std::string comm = match.str(9); // regex bug ! cannot work inplace, need to copy submatch.
            boost::smatch comm_match;
            if (boost::regex_match(comm, comm_match, mCommRe))
            {
                mtId = comm_match.str(2);
                try
                {
                    mtIdType = std::stoi(comm_match.str(1), nullptr, 16) & 0x03;
                    mtAcType =
                            (std::stoi(comm_match.str(1), nullptr, 16) & 0x7C) >> 2;
                } catch (const std::logic_error& e)
                {
                    return MSG_UNPACK_ERR;
                }
                try
                {
                    mtClimbRate = std::stod(comm_match.str(3))
                            * util::math::FPM_2_MS;
                } catch (const std::logic_error& e)
                {
                    mtClimbRate = A_VALUE_NA;
                    tmpFullInfo = false;
                }
                try
                {
                    mtTurnRate = std::stod(comm_match.str(4)) * 3.0; // 1rot = 1 halfcircle / 1 min => 3° / 1s
                } catch (const std::logic_error& e)
                {
                    mtTurnRate = A_VALUE_NA;
                    tmpFullInfo = false;
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
            mtHeading = std::stod(match.str(6));
        } catch (const std::logic_error& e)
        {
            mtHeading = A_VALUE_NA;
            tmpFullInfo = false;
        }
        try
        {
            mtGndSpeed = std::stod(match.str(7)) * util::math::KTS_2_MS;
        } catch (const std::logic_error& e)
        {
            mtGndSpeed = A_VALUE_NA;
            tmpFullInfo = false;
        }
        aircraft::Aircraft ac(mtId, mtGpsPos, mtGndSpeed, mtIdType, mtAcType,
                mtClimbRate, mtTurnRate, mtHeading);
        ac.setFullInfo(tmpFullInfo);
        ac.setTargetT(aircraft::Aircraft::TargetType::FLARM);
        vfrb::VFRB::msAcCont.insertAircraft(ac, prio);
    } else
    {
        return MSG_UNPACK_IGN;
    }
    return MSG_UNPACK_SUC;
}

}  // namespace parser

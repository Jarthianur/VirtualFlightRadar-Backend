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

#include "SbsParser.h"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

#include "../aircraft/Aircraft.h"
#include "../config/Configuration.h"
#include "../data/AircraftContainer.h"
#include "../util/Math.hpp"
#include "../VFRB.h"

namespace parser
{

SbsParser::SbsParser()
        : Parser()
{
}

SbsParser::~SbsParser() noexcept
{
}

std::int32_t SbsParser::unpack(const std::string& cr_msg, std::int32_t prio)
noexcept
{
    /*
     * fields:
     * 4 : id
     * 7 : time
     * 11: altitude
     * 14: latitude
     * 15: longitude
     */
    std::size_t delim;
    std::uint32_t i = 2;
    std::size_t p = 6;

    if (cr_msg.find(',', p) == std::string::npos
            || !(cr_msg.size() > 4 && cr_msg.at(4) == '3'))
    {
        return MSG_UNPACK_IGN;
    }
    while ((delim = cr_msg.find(',', p)) != std::string::npos && i < 16)
    {
        switch (i)
        {
            case 4:
                if (delim - p > 0)
                {
                    mtId = cr_msg.substr(p, delim - p);
                } else
                {
                    return MSG_UNPACK_IGN;
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
                try
                {
                    mtGpsPos.altitude = util::math::dToI(
                            std::stod(cr_msg.substr(p, delim - p))
                                    * util::math::FEET_2_M);
                    if (mtGpsPos.altitude > config::Configuration::filter_maxHeight)
                    {
                        return MSG_UNPACK_IGN;
                    }
                } catch (const std::logic_error& e)
                {
                    return MSG_UNPACK_ERR;
                }
                break;
            case 14:
                try
                {
                    mtGpsPos.latitude = std::stod(cr_msg.substr(p, delim - p));
                } catch (const std::logic_error& e)
                {
                    return MSG_UNPACK_ERR;
                }
                break;
            case 15:
                try
                {
                    mtGpsPos.longitude = std::stod(cr_msg.substr(p, delim - p));
                } catch (const std::logic_error& e)
                {
                    return MSG_UNPACK_ERR;
                }
                break;
            default:
                break;
        }
        i++;
        p = delim + 1;
    }
    aircraft::Aircraft ac(mtId, mtGpsPos);
    ac.setFullInfo(false);
    ac.setTargetT(aircraft::Aircraft::TargetType::TRANSPONDER);
    VFRB::msAcCont.insertAircraft(ac, prio);

    return MSG_UNPACK_SUC;
}

}  // namespace parser

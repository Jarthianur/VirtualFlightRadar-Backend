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

#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include "../config/Configuration.h"
#include "../util/Math.hpp"
#include "../util/Position.h"

namespace parser
{

SbsParser::SbsParser()
        : Parser()
{
}

SbsParser::~SbsParser() noexcept
{
}

bool SbsParser::unpack(const std::string& cr_msg, aircraft::Aircraft& r_ac)
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
    std::size_t p = 6;

    if (cr_msg.find(',', p) == std::string::npos
            || !(cr_msg.size() > 4 && cr_msg.at(4) == '3'))
    {
        return false;
    }
    std::size_t delim;
    std::uint32_t i = 2;
    struct util::GpsPosition pos;
    try
    {
        while ((delim = cr_msg.find(',', p)) != std::string::npos && i < 16)
        {
            switch (i)
            {
                case 4:
                    if (delim - p > 0)
                    {
                        r_ac.setId(cr_msg.substr(p, delim - p));
                    } else
                    {
                        return false;
                    }
                    break;
                    /*case 7:
                     if (delim - p > 7)
                     {
                     mtTime = std::stoi(cr_msg.substr(p, 2)) * 10000;
                     mtTime += std::stoi(cr_msg.substr(p + 3, 2)) * 100;
                     mtTime += std::stoi(cr_msg.substr(p + 6, 2));
                     } else
                     {
                     return false;
                     }
                     break;*/
                case 11:
                    pos.altitude = util::math::dToI(
                            std::stod(cr_msg.substr(p, delim - p))
                                    * util::math::FEET_2_M);
                    if (pos.altitude > config::Configuration::filter_maxHeight)
                    {
                        return false;
                    }
                    break;
                case 14:
                    pos.latitude = std::stod(cr_msg.substr(p, delim - p));
                    break;
                case 15:
                    pos.longitude = std::stod(cr_msg.substr(p, delim - p));
                    break;
                default:
                    break;
            }
            i++;
            p = delim + 1;
        }
    } catch (const std::logic_error& e)
    {
        return false;
    }
    r_ac.setPosition(pos);
    r_ac.setFullInfo(false);
    r_ac.setTargetT(aircraft::Aircraft::TargetType::TRANSPONDER);
    return true;
}

}  // namespace parser

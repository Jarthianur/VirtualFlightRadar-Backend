/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#include "SBSParser.h"

#include <cstddef>
#include <stdexcept>

#include "../config/Configuration.h"
#include "../data/AircraftContainer.h"
#include "../util/Math.hpp"
#include "../vfrb/VFRB.h"

SBSParser::SBSParser()
        : Parser()
{
}

SBSParser::~SBSParser() noexcept
{
}

std::int32_t SBSParser::unpack(const std::string& msg) noexcept
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

    if (msg.find(',', p) == std::string::npos || !(msg.size() > 4 && msg.at(4) == '3'))
    {
        return MSG_UNPACK_IGN;
    }
    while ((delim = msg.find(',', p)) != std::string::npos && i < 16)
    {
        switch (i)
        {
            case 4:
                if (delim - p > 0)
                {
                    mtID = msg.substr(p, delim - p);
                }
                else
                {
                    return MSG_UNPACK_IGN;
                }
                break;
            case 7:
                try
                {
                    if (delim - p > 7)
                    {
                        mtTime = std::stoi(msg.substr(p, 2)) * 10000;
                        mtTime += std::stoi(msg.substr(p + 3, 2)) * 100;
                        mtTime += std::stoi(msg.substr(p + 6, 2));
                    }
                    else
                    {
                        return MSG_UNPACK_IGN;
                    }
                }
                catch (const std::logic_error& e)
                {
                    return MSG_UNPACK_ERR;
                }
                break;
            case 11:
                try
                {
                    mtAlt = Math::dToI(
                            std::stod(msg.substr(p, delim - p)) * Math::feet2m);
                    if (mtAlt > Configuration::filter_maxHeight)
                    {
                        return MSG_UNPACK_IGN;
                    }
                }
                catch (const std::logic_error& e)
                {
                    return MSG_UNPACK_ERR;
                }
                break;
            case 14:
                try
                {
                    mtLat = std::stod(msg.substr(p, delim - p));
                }
                catch (const std::logic_error& e)
                {
                    return MSG_UNPACK_ERR;
                }
                break;
            case 15:
                try
                {
                    mtLong = std::stod(msg.substr(p, delim - p));
                }
                catch (const std::logic_error& e)
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
    Aircraft ac(mtID, mtLat, mtLong, mtAlt);
    ac.setAltQNE();
    ac.setFullInfo(false);
    ac.setTargetT(Aircraft::TargetType::TRANSPONDER);
    VFRB::msAcCont.insertAircraft(ac);

    return MSG_UNPACK_SUC;
}

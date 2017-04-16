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

#include "WindParser.h"

#include <stdexcept>

#include "../data/ClimateData.h"
#include "../util/Math.hpp"
#include "../vfrb/VFRB.h"

WindParser::WindParser()
        : Parser()
{
}

WindParser::~WindParser() noexcept
{
}

std::int32_t WindParser::unpack(const std::string& msg) noexcept
{
    try
    {
        std::int32_t csum = std::stoi(msg.substr(msg.rfind('*', msg.length() - 1) + 1, 2),
                                      nullptr, 16);
        if (csum != Math::checksum(msg.c_str(), msg.length()))
        {
            return MSG_UNPACK_IGN;
        }
    }
    catch (const std::logic_error& e)
    {
        return MSG_UNPACK_ERR;
    }

    if (msg.find("WIMWV") != std::string::npos)
    {
        VFRB::msClimateData.insertWV(msg);
    }
    else if (msg.find("WIMDA") != std::string::npos)
    {
        try
        {
            mtB = msg.find('B') - 1;
            mtS = msg.substr(0, mtB).find_last_of(',') + 1;
            mtSubLen = mtB - mtS;
            mtPress = std::stod(msg.substr(mtS, mtSubLen), &mtNumIdx) * 1000.0;
            if (mtNumIdx == mtSubLen)
            {
                VFRB::msClimateData.setPress(mtPress);
            }
            else
            {
                return MSG_UNPACK_ERR;
            }
        }
        catch (std::logic_error& e)
        {
            return MSG_UNPACK_ERR;
        }
        try
        {
            mtB = msg.find('C') - 1;
            mtS = msg.substr(0, mtB).find_last_of(',') + 1;
            mtSubLen = mtB - mtS;
            mtTemp = std::stod(msg.substr(mtS, mtSubLen), &mtNumIdx) * 1000.0;
            if (mtNumIdx == mtSubLen)
            {
                VFRB::msClimateData.setTemp(mtTemp);
            }
            else
            {
                return MSG_UNPACK_ERR;
            }
        }
        catch (std::logic_error& e)
        {
            return MSG_UNPACK_ERR;
        }
    }
    else
    {
        return MSG_UNPACK_IGN;
    }
    return MSG_UNPACK_SUC;
}

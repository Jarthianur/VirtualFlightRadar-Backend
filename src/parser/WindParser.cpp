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

#include "WindParser.h"

#include <stdexcept>

#include "../data/ClimateData.h"
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
    if (msg.substr(1, 5).compare("WIMWV") == 0)
    {
        VFRB::msClimateData.insertWV(msg);
    }
    else if (msg.substr(1, 5).compare("WIMDA") == 0)
    {
        try
        {
            mtB = msg.find('B') - 1;
            mtS = msg.substr(0, mtB).find_last_of(',') + 1;
            VFRB::msClimateData.setPress(std::stod(msg.substr(mtS, mtB - mtS)) * 1000.0);
        }
        catch (std::logic_error& e)
        {
            VFRB::msClimateData.setPress();
        }
        try
        {
            mtB = msg.find('C') - 1;
            mtS = msg.substr(0, mtB).find_last_of(',') + 1;
            VFRB::msClimateData.setTemp(std::stod(msg.substr(mtS, mtB - mtS)));
        }
        catch (std::logic_error& e)
        {
            VFRB::msClimateData.setTemp();
        }
    }
    else
    {
        return MSG_UNPACK_IGN;
    }
    return MSG_UNPACK_SUC;
}

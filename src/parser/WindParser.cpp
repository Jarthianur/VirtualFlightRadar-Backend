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

WindParser::~WindParser()
{
}

int32_t WindParser::unpack(const std::string& sentence)
{
    if (sentence.substr(1, 5).compare("WIMWV") == 0)
    {
        VFRB::climate_data.insertWV(sentence);
    }
    else if (sentence.substr(1, 5).compare("WIMDA") == 0)
    {
        try
        {
            b = sentence.find('B') - 1;
            s = sentence.substr(0, b).find_last_of(',') + 1;
            VFRB::climate_data.setPress(std::stod(sentence.substr(s, b - s)) * 1000.0);
        }
        catch (std::logic_error& e)
        {
            VFRB::climate_data.setPress();
        }
        try
        {
            b = sentence.find('C') - 1;
            s = sentence.substr(0, b).find_last_of(',') + 1;
            VFRB::climate_data.setTemp(std::stod(sentence.substr(s, b - s)));
        }
        catch (std::logic_error& e)
        {
            VFRB::climate_data.setTemp();
        }
    }
    else
    {
        return MSG_UNPACK_IGN;
    }
    return MSG_UNPACK_SUC;
}

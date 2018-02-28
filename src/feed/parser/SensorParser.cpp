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

#include "SensorParser.h"

#include <stdexcept>

#include "../data/AtmosphereData.h"
#include "../util/Math.hpp"

namespace parser
{
SensorParser::SensorParser() : Parser()
{}

SensorParser::~SensorParser() noexcept
{}

bool SensorParser::unpack(const std::string& cr_msg,
                          data::object::Climate& rClimate) noexcept
{
    try
    {
        std::int32_t csum
            = std::stoi(cr_msg.substr(cr_msg.rfind('*') + 1, 2), nullptr, 16);
        if(csum != util::math::checksum(cr_msg.c_str(), cr_msg.length()))
        {
            return false;
        }
        if(cr_msg.find("MDA") != std::string::npos)
        {
            std::size_t tmpB   = cr_msg.find('B') - 1;
            std::size_t tmpS   = cr_msg.substr(0, tmpB).find_last_of(',') + 1;
            std::size_t subLen = tmpB - tmpS;
            std::size_t numIdx;
            double tmpPress = std::stod(cr_msg.substr(tmpS, subLen), &numIdx) * 1000.0;
            if(numIdx == subLen)
            {
                rClimate.mAtmosphere.setMdaStr(cr_msg + "\n");
                rClimate.mAtmosphere.setPressure(tmpPress);
            }
            else
            {
                return false;
            }
        }
        else if(cr_msg.find("MWV") != std::string::npos)
        {
            rClimate.mWind.setMwvStr(cr_msg + "\n");
        }
        else
        {
            return false;
        }
    }
    catch(const std::logic_error&)
    {
        return false;
    }
    return true;
}

}  // namespace parser

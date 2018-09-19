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

#include "AtmosphereParser.h"

#include <cstddef>
#include <stdexcept>

#include "../../Math.hpp"

namespace feed
{
namespace parser
{
AtmosphereParser::AtmosphereParser() : Parser<object::Atmosphere>()
{}

AtmosphereParser::~AtmosphereParser() noexcept
{}

bool AtmosphereParser::unpack(const std::string& crStr, object::Atmosphere& rAtmos) noexcept
{
    try
    {
        return (std::stoi(crStr.substr(crStr.rfind('*') + 1, 2), nullptr, 16)
                    == math::checksum(crStr.c_str(), crStr.length())
                && parseAtmosphere(crStr, rAtmos));
    }
    catch(const std::logic_error&)
    {
        return false;
    }
}

bool AtmosphereParser::parseAtmosphere(const std::string& crStr, object::Atmosphere& rAtmos)
{
    bool valid = false;
    if(crStr.find("MDA") != std::string::npos)
    {
        std::size_t tmpB   = crStr.find('B') - 1;
        std::size_t tmpS   = crStr.substr(0, tmpB).find_last_of(',') + 1;
        std::size_t subLen = tmpB - tmpS;
        std::size_t numIdx;
        double tmpPress = std::stod(crStr.substr(tmpS, subLen), &numIdx) * 1000.0;
        if((valid = (numIdx == subLen)))
        {
            rAtmos.set_serialized(std::string(crStr));
            rAtmos.set_pressure(tmpPress);
        }
    }
    return valid;
}
}  // namespace parser
}  // namespace feed

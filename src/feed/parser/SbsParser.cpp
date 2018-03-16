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

#include <limits>
#include <stdexcept>

#include "../../Math.hpp"
#include "../../object/Position.h"

/// @def SBS_FIELD_ID
/// Field number of aircraft id
#define SBS_FIELD_ID 4

/// @def SBS_FIELD_TIME
/// Field number of time
#define SBS_FIELD_TIME 7

/// @var SBS_FIELD_ALT
/// Field number of altitude
#define SBS_FIELD_ALT 11

/// @def BS_FIELD_LAT
/// Field number of latitude
#define SBS_FIELD_LAT 14

/// @def SBS_FIELD_LON
/// Field number of longitude
#define SBS_FIELD_LON 15

using namespace object;

namespace feed
{
namespace parser
{
SbsParser::SbsParser() : SbsParser(std::numeric_limits<std::int32_t>::max())
{}

SbsParser::SbsParser(std::int32_t vMaxHeight) : Parser<Aircraft>(), mMaxHeight(vMaxHeight)
{}

SbsParser::~SbsParser() noexcept
{}

bool SbsParser::unpack(const std::string& crStr, Aircraft& rAircraft) noexcept
{
    std::size_t p   = 6, delim;
    std::uint32_t i = 2;
    Position pos;

    if(crStr.find(',', p) == std::string::npos || !(crStr.size() > 4 && crStr[4] == '3'))
    {
        return false;
    }
    while((delim = crStr.find(',', p)) != std::string::npos && i < 16)
    {
        if(!parseField(i++, crStr.substr(p, delim - p), pos, rAircraft))
        {
            return false;
        }
        p = delim + 1;
    }
    rAircraft.setPosition(pos);
    rAircraft.setTargetType(Aircraft::TargetType::TRANSPONDER);
    rAircraft.setAircraftType(Aircraft::AircraftType::POWERED_AIRCRAFT);
    rAircraft.setIdType(Aircraft::IdType::ICAO);
    return i == 15 && pos.altitude <= mMaxHeight;
}

bool SbsParser::parseField(std::uint32_t vField, const std::string& crStr,
                           Position& rPosition, Aircraft& rAircraft) noexcept
{
    bool valid = !crStr.empty();
    if(valid)
    {
        try
        {
            switch(vField)
            {
                case SBS_FIELD_ID:
                    rAircraft.setId(crStr);
                    break;
                case SBS_FIELD_ALT:
                    rPosition.altitude
                        = math::doubleToInt(std::stod(crStr) * math::FEET_2_M);
                    break;
                case SBS_FIELD_LAT:
                    rPosition.latitude = std::stod(crStr);
                    break;
                case SBS_FIELD_LON:
                    rPosition.longitude = std::stod(crStr);
                    break;
                default:
                    break;
            }
        }
        catch(const std::logic_error&)
        {
            valid = false;
        }
    }
    return valid;
}
}  // namespace parser
}  // namespace feed

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
#include <limits>
#include <stdexcept>

#include "object/GpsPosition.h"
#include "util/math.hpp"

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
std::int32_t SbsParser::s_maxHeight = std::numeric_limits<std::int32_t>::max();

SbsParser::SbsParser() : Parser<Aircraft>() {}

bool SbsParser::unpack(const std::string& sentence, Aircraft& aircraft) noexcept
{
    std::size_t   p = 6, delim;
    std::uint32_t i = 2;
    Position      pos;

    if (sentence.find(',', p) == std::string::npos || !(sentence.size() > 4 && sentence[4] == '3'))
    {
        return false;
    }
    while ((delim = sentence.find(',', p)) != std::string::npos && i < 16)
    {
        if (!parseField(i++, sentence.substr(p, delim - p), pos, aircraft))
        {
            return false;
        }
        p = delim + 1;
    }
    aircraft.set_position(pos);
    aircraft.set_targetType(Aircraft::TargetType::TRANSPONDER);
    aircraft.set_aircraftType(Aircraft::AircraftType::POWERED_AIRCRAFT);
    aircraft.set_idType(Aircraft::IdType::ICAO);
    return i == 16 && pos.altitude <= s_maxHeight;
}

bool SbsParser::parseField(std::uint32_t fieldNr, const std::string& field, Position& position,
                           Aircraft& aircraft) noexcept
{
    try
    {
        switch (fieldNr)
        {
            case SBS_FIELD_ID: aircraft.set_id(field); break;
            case SBS_FIELD_TIME:
                aircraft.set_timeStamp(TimeStamp<timestamp::DateTimeImplBoost>(
                    field, timestamp::Format::HH_MM_SS_FFF));
                break;
            case SBS_FIELD_ALT:
                position.altitude = math::doubleToInt(std::stod(field) * math::FEET_2_M);
                break;
            case SBS_FIELD_LAT: position.latitude = std::stod(field); break;
            case SBS_FIELD_LON: position.longitude = std::stod(field); break;
            default: break;
        }
    }
    catch (const std::logic_error&)
    {
        return false;
    }
    return true;
}
}  // namespace parser
}  // namespace feed

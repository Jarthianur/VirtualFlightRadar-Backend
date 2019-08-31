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

#include "feed/parser/SbsParser.h"

#include <limits>
#include <stdexcept>

#include "object/GpsPosition.h"
#include "util/math.hpp"

using namespace vfrb::object;

namespace vfrb::feed::parser
{
s32 SbsParser::s_maxHeight = std::numeric_limits<s32>::max();

SbsParser::SbsParser() : Parser<Aircraft>() {}

Aircraft SbsParser::unpack(str&& sentence, u32 priority) const
{
    u32                                i = 2;
    Location                           loc;
    str                                id;
    Timestamp<time::DateTimeImplBoost> ts;

    try
    {
        if (usize p = 6, delim = 0;
            sentence.size() > 4 && sentence[4] == '3' && sentence.find(',', p) != str::npos)
        {
            while ((delim = sentence.find(',', p)) != str::npos && i < 16)
            {
                switch (i)
                {
                    case SBS_FIELD_ID:
                        id = sentence.substr(p, delim - p);
                        if (id.size() >= Aircraft::ID_SIZE)
                        {
                            throw error::UnpackError();
                        }
                        break;
                    case SBS_FIELD_TIME:
                        ts = Timestamp<time::DateTimeImplBoost>(sentence.substr(p, delim - p),
                                                                time::Format::HH_MM_SS_FFF);
                        break;
                    case SBS_FIELD_ALT:
                        loc.altitude =
                            math::doubleToInt(std::stod(sentence.substr(p, delim - p)) * math::FEET_2_M);
                        break;
                    case SBS_FIELD_LAT: loc.latitude = std::stod(sentence.substr(p, delim - p)); break;
                    case SBS_FIELD_LON: loc.longitude = std::stod(sentence.substr(p, delim - p)); break;
                    default: break;
                }
                p = delim + 1;
                i += 1;
            }
        }
        if (i == 16 && loc.altitude <= s_maxHeight)
        {
            return {priority, id, Aircraft::IdType::ICAO, Aircraft::AircraftType::POWERED_AIRCRAFT, loc, ts};
        }
    }
    catch ([[maybe_unused]] std::logic_error const&)
    {}
    catch ([[maybe_unused]] object::error::TimestampParseError const&)
    {}
    throw error::UnpackError();
}
}  // namespace vfrb::feed::parser

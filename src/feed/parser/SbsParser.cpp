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

#include <cstddef>
#include <limits>
#include <stdexcept>

#include "object/GpsPosition.h"
#include "util/math.hpp"

using namespace object;

namespace feed
{
namespace parser
{
std::int32_t SbsParser::s_maxHeight = std::numeric_limits<std::int32_t>::max();

SbsParser::SbsParser() : Parser<Aircraft>() {}

Aircraft SbsParser::unpack(const std::string& sentence, std::uint32_t priority) const
{
    std::size_t                        p = 6, delim;
    std::uint32_t                      i = 2;
    Location                           loc;
    std::string                        id;
    Timestamp<time::DateTimeImplBoost> ts;

    if (sentence.size() > 4 && sentence[4] == '3' && sentence.find(',', p) != std::string::npos)
    {
        while ((delim = sentence.find(',', p)) != std::string::npos && i < 16)
        {
            try
            {
                switch (i)
                {
                    case SBS_FIELD_ID:
                        id = sentence.substr(p, delim - p);
                        if (id.size() >= Aircraft::ID_SIZE)
                        {
                            throw std::out_of_range("");
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
            }
            catch (const std::logic_error&)
            {
                break;
            }
            p = delim + 1;
            i += 1;
        }
    }
    if (i == 16 && loc.altitude <= s_maxHeight)
    {
        // relies on TargetType::TRANSPONDER in ctor
        return {priority, id, Aircraft::IdType::ICAO, Aircraft::AircraftType::POWERED_AIRCRAFT, loc, ts};
    }
    throw UnpackError();
}

}  // namespace parser
}  // namespace feed

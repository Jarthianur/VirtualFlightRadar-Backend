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

#include "object/GpsPosition.h"
#include "util/math.hpp"
#include "util/string_utils.hpp"

using namespace vfrb::object;
using namespace vfrb::str_util;

namespace vfrb::feed::parser
{
SbsParser::SbsParser(s32 maxHeight) : Parser<Aircraft>(), m_maxHeight(maxHeight) {}

Aircraft SbsParser::unpack(Str&& sentence, u32 priority) const
{
    u32              i = 2;
    Location         loc;
    std::string_view id;
    Timestamp        ts;

    try
    {
        if (usize p = 6, delim = 0;
            sentence.size() > 4 && sentence[4] == '3' && sentence.find(',', p) != Str::npos)
        {
            while ((delim = sentence.find(',', p)) != Str::npos && i < 16)
            {
                switch (i)
                {
                    case SBS_FIELD_ID:
                        id = std::string_view(sentence.c_str() + p, delim - p);
                        if (id.size() != Aircraft::ID_LEN)
                        {
                            throw error::UnpackError();
                        }
                        break;
                    case SBS_FIELD_TIME:
                        ts = Timestamp(std::string_view(sentence.c_str() + p, delim - p));
                        break;
                    case SBS_FIELD_ALT:
                        if (auto [v, ec] = Convert<f64>(sentence.c_str() + p, sentence.c_str() + delim);
                            ec == Errc::OK)
                        {
                            loc.altitude = math::DoubleToInt(v * math::FEET_2_M);
                        }
                        else
                        {
                            throw error::UnpackError();
                        }
                        break;
                    case SBS_FIELD_LAT:
                        if (auto ec =
                                Convert<f64>(sentence.c_str() + p, sentence.c_str() + delim, loc.latitude);
                            ec == Errc::ERR)
                        {
                            throw error::UnpackError();
                        }
                        break;
                    case SBS_FIELD_LON:
                        if (auto ec =
                                Convert<f64>(sentence.c_str() + p, sentence.c_str() + delim, loc.longitude);
                            ec == Errc::ERR)
                        {
                            throw error::UnpackError();
                        }
                        break;
                    default: break;
                }
                p = delim + 1;
                i += 1;
            }
        }
        if (i == 16 && loc.altitude <= m_maxHeight)
        {
            return {priority, id, Aircraft::IdType::ICAO, Aircraft::AircraftType::POWERED_AIRCRAFT, loc, ts};
        }
    }
    catch ([[maybe_unused]] object::error::TimestampParseError const&)
    {}
    throw error::UnpackError();
}
}  // namespace vfrb::feed::parser

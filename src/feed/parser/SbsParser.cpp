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
SbsParser::SbsParser(s32 maxHeight) : Parser<CAircraft>(), m_maxHeight(maxHeight) {}

CAircraft SbsParser::unpack(Str&& sentence, u32 priority) const
{
    u32              i = 2;
    SLocation         loc;
    std::string_view id;
    CTimestamp        ts;

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
                        if (id.size() != CAircraft::ID_LEN)
                        {
                            throw error::UnpackError();
                        }
                        break;
                    case SBS_FIELD_TIME:
                        ts = CTimestamp(std::string_view(sentence.c_str() + p, delim - p));
                        break;
                    case SBS_FIELD_ALT:
                        if (auto [v, ec] = Convert<f64>(sentence.c_str() + p, sentence.c_str() + delim);
                            ec == EErrc::OK)
                        {
                            loc.Altitude = math::DoubleToInt(v * math::FEET_2_M);
                        }
                        else
                        {
                            throw error::UnpackError();
                        }
                        break;
                    case SBS_FIELD_LAT:
                        if (auto ec =
                                Convert<f64>(sentence.c_str() + p, sentence.c_str() + delim, loc.Latitude);
                            ec == EErrc::ERR)
                        {
                            throw error::UnpackError();
                        }
                        break;
                    case SBS_FIELD_LON:
                        if (auto ec =
                                Convert<f64>(sentence.c_str() + p, sentence.c_str() + delim, loc.Longitude);
                            ec == EErrc::ERR)
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
        if (i == 16 && loc.Altitude <= m_maxHeight)
        {
            return {priority, id, CAircraft::EIdType::ICAO, CAircraft::EAircraftType::POWERED_AIRCRAFT, loc, ts};
        }
    }
    catch ([[maybe_unused]] object::error::CTimestampParseError const&)
    {}
    throw error::UnpackError();
}
}  // namespace vfrb::feed::parser

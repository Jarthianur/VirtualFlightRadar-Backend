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

#include "math/math.hpp"
#include "object/GpsPosition.h"
#include "util/string_utils.hpp"

using namespace vfrb::object;
using namespace vfrb::str_util;

namespace vfrb::feed::parser
{
CSbsParser::CSbsParser(s32 maxHeight_) : IParser<CAircraft>(), m_maxHeight(maxHeight_) {}

CAircraft CSbsParser::Parse(Str&& str_, u32 prio_) const
{
    u32        i = 2;
    SLocation  loc;
    StrView    id;
    CTimestamp ts;

    try
    {
        if (usize p = 6, delim = 0; str_.size() > 4 && str_[4] == '3' && str_.find(',', p) != Str::npos)
        {
            while ((delim = str_.find(',', p)) != Str::npos && i < 16)
            {
                switch (i)
                {
                    case SBS_FIELD_ID:
                        id = StrView(str_.c_str() + p, delim - p);
                        if (id.size() != CAircraft::ID_LEN)
                        {
                            throw error::CParseError();
                        }
                        break;
                    case SBS_FIELD_TIME: ts = CTimestamp(StrView(str_.c_str() + p, delim - p)); break;
                    case SBS_FIELD_ALT:
                        if (auto [v, ec] = Convert<f64>(str_.c_str() + p, str_.c_str() + delim);
                            ec == EErrc::OK)
                        {
                            loc.Altitude = math::DoubleToInt(v * math::FEET_2_M);
                        }
                        else
                        {
                            throw error::CParseError();
                        }
                        break;
                    case SBS_FIELD_LAT:
                        if (auto ec = Convert<f64>(str_.c_str() + p, str_.c_str() + delim, loc.Latitude);
                            ec == EErrc::ERR)
                        {
                            throw error::CParseError();
                        }
                        break;
                    case SBS_FIELD_LON:
                        if (auto ec = Convert<f64>(str_.c_str() + p, str_.c_str() + delim, loc.Longitude);
                            ec == EErrc::ERR)
                        {
                            throw error::CParseError();
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
            return {prio_, id, CAircraft::EIdType::ICAO, CAircraft::EAircraftType::POWERED_AIRCRAFT, loc, ts};
        }
    }
    catch ([[maybe_unused]] object::error::CTimestampParseError const&)
    {}
    throw error::CParseError();
}
}  // namespace vfrb::feed::parser

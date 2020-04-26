/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "feed/parser/CSbsParser.hpp"

#include "math/math.hpp"
#include "object/CGpsPosition.hpp"
#include "util/string_utils.hpp"

using namespace vfrb::object;
using namespace vfrb::str_util;

namespace vfrb::feed::parser
{
CSbsParser::CSbsParser(s32 maxHeight_) : IParser<CAircraft>(), m_maxHeight(maxHeight_) {}

CAircraft CSbsParser::Parse(String&& str_, u32 prio_) const {
    u32        i = 2;
    SLocation  loc;
    StringView id;
    CTimestamp ts;

    try {
        if (usize p = 6, delim = 0; str_.size() > 4 && str_[4] == '3' && str_.find(',', p) != String::npos) {
            while ((delim = str_.find(',', p)) != String::npos && i < 16) {
                switch (i) {
                    case SBS_FIELD_ID:
                        id = StringView(str_.c_str() + p, delim - p);
                        if (id.size() != CAircraft::ID_LEN) {
                            throw error::CParseError();
                        }
                        break;
                    case SBS_FIELD_TIME: ts = CTimestamp(StringView(str_.c_str() + p, delim - p)); break;
                    case SBS_FIELD_ALT:
                        if (auto [v, ec] = Convert<f64>(str_.c_str() + p, str_.c_str() + delim);
                            ec == EErrc::OK) {
                            loc.Altitude = math::DoubleToInt(v * math::FEET_2_M);
                        } else {
                            throw error::CParseError();
                        }
                        break;
                    case SBS_FIELD_LAT:
                        if (auto ec = Convert<f64>(str_.c_str() + p, str_.c_str() + delim, loc.Latitude);
                            ec == EErrc::ERR) {
                            throw error::CParseError();
                        }
                        break;
                    case SBS_FIELD_LON:
                        if (auto ec = Convert<f64>(str_.c_str() + p, str_.c_str() + delim, loc.Longitude);
                            ec == EErrc::ERR) {
                            throw error::CParseError();
                        }
                        break;
                    default: break;
                }
                p = delim + 1;
                i += 1;
            }
        }
        if (i == 16 && loc.Altitude <= m_maxHeight) {
            return {prio_, id, CAircraft::EIdType::ICAO, CAircraft::EAircraftType::POWERED_AIRCRAFT, loc, ts};
        }
    } catch ([[maybe_unused]] object::error::CTimestampParseError const&) {
    }
    throw error::CParseError();
}
}  // namespace vfrb::feed::parser

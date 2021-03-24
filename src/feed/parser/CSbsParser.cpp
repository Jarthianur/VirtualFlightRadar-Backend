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

#include "math/Math.hpp"
#include "object/CAircraft.hpp"
#include "object/CGpsPosition.hpp"
#include "object/CTimestamp.hpp"
#include "util/StringUtils.hpp"

using vfrb::object::CAircraft;
using vfrb::object::CTimestamp;
using vfrb::str_util::Convert;
using vfrb::str_util::ConvertInto;

namespace vfrb::feed::parser
{
namespace
{
constexpr auto PROTO_START_INDEX = 6UL;
constexpr auto PROTO_ITER_COUNT  = 16UL;
}  // namespace

CSbsParser::CSbsParser(i32 maxHeight_) : IParser<SbsResult>(), m_maxHeight(maxHeight_) {}

auto
CSbsParser::parsePositionReport(String&& str_) const -> Pair<CAircraft::IdString, SPositionUpdate> {
    SPositionUpdate     pos{};
    CAircraft::IdString id;
    usize               p = PROTO_START_INDEX, delim = 0, i = 2;

    while ((delim = str_.find(',', p)) != String::npos && i < PROTO_ITER_COUNT) {
        switch (i) {
            case SBS_FIELD_ID: {
                StringView sv(str_.c_str() + p, delim - p);
                if (sv.size() != CAircraft::ID_LEN) {
                    throw error::CParseError();
                }
                id = sv;
            } break;
            case SBS_FIELD_TIME: pos.Timestamp = CTimestamp(StringView(str_.c_str() + p, delim - p)); break;
            case SBS_FIELD_ALT:
                if (auto [v, ec] = Convert<f64>(str_.c_str() + p, str_.c_str() + delim); ec == EErrc::OK) {
                    pos.Location.Altitude = math::DoubleToInt(v * math::FEET_2_M);
                } else {
                    throw error::CParseError();
                }
                break;
            case SBS_FIELD_LAT:
                if (auto ec = ConvertInto<f64>(str_.c_str() + p, str_.c_str() + delim, pos.Location.Latitude);
                    ec == EErrc::ERR) {
                    throw error::CParseError();
                }
                break;
            case SBS_FIELD_LON:
                if (auto ec =
                        ConvertInto<f64>(str_.c_str() + p, str_.c_str() + delim, pos.Location.Longitude);
                    ec == EErrc::ERR) {
                    throw error::CParseError();
                }
                break;
            default: break;
        }
        p = delim + 1;
        i += 1;
    }
    if (i == PROTO_ITER_COUNT && pos.Location.Altitude <= m_maxHeight) {
        return {id, pos};
    }
    throw error::CParseError();
}

auto
CSbsParser::parseVelocityReport(String&& str_) const -> Pair<CAircraft::IdString, SMovementUpdate> {
    SMovementUpdate     mov{};
    CAircraft::IdString id;
    usize               p = PROTO_START_INDEX, delim = 0, i = 2;

    while ((delim = str_.find(',', p)) != String::npos && i < PROTO_ITER_COUNT) {
        switch (i) {
            case SBS_FIELD_ID: {
                StringView sv(str_.c_str() + p, delim - p);
                if (sv.size() != CAircraft::ID_LEN) {
                    throw error::CParseError();
                }
                id = sv;
            } break;
            case SBS_FIELD_TIME: mov.Timestamp = CTimestamp(StringView(str_.c_str() + p, delim - p)); break;
            case SBS_FIELD_GNDSPD:
                if (auto [v, ec] = Convert<f64>(str_.c_str() + p, str_.c_str() + delim); ec == EErrc::OK) {
                    mov.Movement.GndSpeed = math::DoubleToInt(v);  // What unit is reported from SBS?
                } else {
                    throw error::CParseError();
                }
                break;
            case SBS_FIELD_TRACK:
                if (auto ec = ConvertInto<f64>(str_.c_str() + p, str_.c_str() + delim, mov.Movement.Heading);
                    ec == EErrc::ERR) {
                    throw error::CParseError();
                }
                break;
            case SBS_FIELD_VERTRATE:
                if (auto [v, ec] = Convert<i32>(str_.c_str() + p, str_.c_str() + delim); ec == EErrc::OK) {
                    mov.Movement.ClimbRate = v;  // What unit is reported from SBS? ft per what?
                } else {
                    throw error::CParseError();
                }
                break;
            default: break;
        }
        p = delim + 1;
        i += 1;
    }
    if (i == PROTO_ITER_COUNT) {
        return {id, mov};
    }
    throw error::CParseError();
}

auto
CSbsParser::Parse(String&& str_, [[maybe_unused]] u32) const -> SbsResult {
    if (str_.size() > 4) {
        try {
            if (str_[4] == '3') {
                return parsePositionReport(std::move(str_));
            } else if (str_[4] == '4') {
                return parseVelocityReport(std::move(str_));
            }
        } catch ([[maybe_unused]] object::error::CTimestampParseError const&) {
        }
    }
    throw error::CParseError();
}
}  // namespace vfrb::feed::parser

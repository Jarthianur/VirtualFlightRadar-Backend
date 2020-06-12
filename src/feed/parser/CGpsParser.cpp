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

#include "feed/parser/CGpsParser.hpp"

#include "math/Math.hpp"
#include "object/CGpsPosition.hpp"
#include "object/CTimestamp.hpp"
#include "util/StringUtils.hpp"

using vfrb::object::CGpsPosition;
using vfrb::object::CTimestamp;
using vfrb::str_util::MatchChecksum;
using vfrb::str_util::AsStrView;

namespace vfrb::feed::parser
{
CGpsParser::CGpsParser()
    : m_gpggaRe(
          "^\\$[A-Z]{2}GGA,(\\d{6}),"    // time
          "(\\d{4}\\.\\d{3,4}),([NS]),"  // latitude, direction
          "(\\d{5}\\.\\d{3,4}),([EW]),"  // longitude, direction
          "(\\d),(\\d{2}),"              // fix quality, number of satellites
          "(\\d+(?:\\.\\d+)?),"          // dilution
          "(\\d+(?:\\.\\d+)?),M,"        // altitude
          "(\\d+(?:\\.\\d+)?)"           // geoid
          ",M,,\\*[0-9A-F]{2}\\s*?$",
          std::regex::optimize | std::regex::icase) {}

auto CGpsParser::Parse(String&& str_, u32 prio_) const -> CGpsPosition {
    try {
        if (std::cmatch match; str_util::MatchChecksum({str_.c_str(), str_.length()}) &&
                               std::regex_match(str_.c_str(), match, m_gpggaRe)) {
            return parsePosition(match, prio_);
        }
    } catch ([[maybe_unused]] str_util::error::CConversionError const&) {
    } catch ([[maybe_unused]] object::error::CTimestampParseError const&) {
    }
    throw error::CParseError();
}

auto CGpsParser::parsePosition(std::cmatch const& match_, u32 prio_) -> CGpsPosition {
    auto latitude = math::DmToDeg(str_util::Parse<f64>(match_[RE_GGA_LAT]));
    if (match_[RE_GGA_LAT_DIR] == "S") {
        latitude = -latitude;
    }
    auto longitude = math::DmToDeg(str_util::Parse<f64>(match_[RE_GGA_LON]));
    if (match_[RE_GGA_LON_DIR] == "W") {
        longitude = -longitude;
    }
    auto altitude = math::DoubleToInt(str_util::Parse<f64>(match_[RE_GGA_ALT]));
    return {prio_,
            {latitude, longitude, altitude},
            str_util::Parse<f64>(match_[RE_GGA_GEOID]),
            str_util::Parse<f64>(match_[RE_GGA_DIL]),
            str_util::Parse<u8>(match_[RE_GGA_SAT]),
            str_util::Parse<s8>(match_[RE_GGA_FIX]),
            CTimestamp(AsStrView(match_[RE_GGA_TIME]))};
}
}  // namespace vfrb::feed::parser

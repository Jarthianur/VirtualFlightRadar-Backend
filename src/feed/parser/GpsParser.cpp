/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#include "feed/parser/GpsParser.h"

#include "object/GpsPosition.h"
#include "object/Timestamp.h"
#include "util/math.hpp"
#include "util/string_utils.hpp"

using namespace vfrb::object;
using namespace vfrb::str_util;

namespace vfrb::feed::parser
{
std::regex const GpsParser::s_GPGGA_RE(
    "^\\$[A-Z]{2}GGA,(\\d{6}),(\\d{4}\\.\\d{3,4}),([NS]),(\\d{5}\\.\\d{3,4}),([EW]),(\\d),(\\d{2}),(\\d+(?:\\.\\d+)?),(\\d+(?:\\.\\d+)?),M,(\\d+(?:\\.\\d+)?),M,,\\*[0-9A-F]{2}\\s*?$",
    std::regex::optimize | std::regex::icase);

CGpsPosition GpsParser::unpack(Str&& sentence, u32 priority) const
{
    try
    {
        if (std::cmatch match; MatchChecksum({sentence.c_str(), sentence.length()}) &&
                               std::regex_match(sentence.c_str(), match, s_GPGGA_RE))
        {
            return parsePosition(match, priority);
        }
    }
    catch ([[maybe_unused]] str_util::error::CConversionError const&)
    {}
    catch ([[maybe_unused]] object::error::CTimestampParseError const&)
    {}
    throw error::UnpackError();
}

CGpsPosition GpsParser::parsePosition(std::cmatch const& match, u32 priority) const
{
    auto latitude = math::DmToDeg(Parse<f64>(match[RE_GGA_LAT]));
    if (match[RE_GGA_LAT_DIR] == "S")
    {
        latitude = -latitude;
    }
    auto longitude = math::DmToDeg(Parse<f64>(match[RE_GGA_LON]));
    if (match[RE_GGA_LON_DIR] == "W")
    {
        longitude = -longitude;
    }
    auto altitude = math::DoubleToInt(Parse<f64>(match[RE_GGA_ALT]));
    return {priority,
            {latitude, longitude, altitude},
            Parse<f64>(match[RE_GGA_GEOID]),
            Parse<f64>(match[RE_GGA_DIL]),
            Parse<u8>(match[RE_GGA_SAT]),
            Parse<s8>(match[RE_GGA_FIX]),
            CTimestamp(AsStrView(match[RE_GGA_TIME]))};
}
}  // namespace vfrb::feed::parser

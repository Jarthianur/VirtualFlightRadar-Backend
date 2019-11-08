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

#include "feed/parser/AprsParser.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

#include "object/Timestamp.h"
#include "util/math.hpp"
#include "util/string_utils.hpp"

using namespace vfrb::object;
using namespace vfrb::str_util;

namespace vfrb::feed::parser
{
AprsParser::AprsParser(s32 maxHeight)
    : Parser<Aircraft>(),
      m_APRS_RE("^(?:\\S+?)>APRS,\\S+?(?:,\\S+?)?:/"
                "(\\d{6})h"                               // time
                "(\\d{4}\\.\\d{2})([NS])[\\S\\s]+?"       // lat, lat-dir
                "(\\d{5}\\.\\d{2})([EW])[\\S\\s]+?"       // lon, lon-dir
                "(?:(\\d{3})/(\\d{3}))?/A=(\\d{6})\\s+?"  // head.gnd-spd,alt
                "(?:[\\S\\s]+?)?"
                "id([0-9A-F]{2})([0-9A-F]{6})\\s?"                // type,id
                "(?:([\\+-]\\d{3})fpm\\s+?)?"                     // climb
                "(?:([\\+-]\\d+?\\.\\d+?)rot)?(?:[\\S\\s]+?)?$",  // turn
                std::regex::optimize | std::regex::icase),
      m_maxHeight(maxHeight)
{}

Aircraft AprsParser::unpack(Str&& sentence, u32 priority) const
{
    std::cmatch match;
    if ((!sentence.empty() && sentence[0] == '#') || !std::regex_match(sentence.c_str(), match, m_APRS_RE))
    {
        throw error::UnpackError();
    }
    try
    {
        auto [id, idT, aT] = parseComment(match);
        // relies on TargetType::FLARM in ctor
        return {priority, id, idT, aT, parseLocation(match), parseMovement(match), parseTimeStamp(match)};
    }
    catch ([[maybe_unused]] str_util::error::ConversionError const&)
    {}
    catch ([[maybe_unused]] object::error::TimestampParseError const&)
    {}
    throw error::UnpackError();
}

Location AprsParser::parseLocation(std::cmatch const& match) const
{
    Location pos;
    pos.latitude = math::DmToDeg(Parse<f64>(match[RE_APRS_LAT]));
    if (ToStrView(match[RE_APRS_LAT_DIR]) == "S")
    {
        pos.latitude = -pos.latitude;
    }
    pos.longitude = math::DmToDeg(Parse<f64>(match[RE_APRS_LON]));
    if (ToStrView(match[RE_APRS_LON_DIR]) == "W")
    {
        pos.longitude = -pos.longitude;
    }
    pos.altitude = math::DoubleToInt(Parse<f64>(match[RE_APRS_ALT]) * math::FEET_2_M);
    if (pos.altitude <= m_maxHeight)
    {
        return pos;
    }
    throw error::UnpackError();
}

AprsParser::AircraftInfo AprsParser::parseComment(std::cmatch const& match) const
{
    return {ToStrView(match[RE_APRS_ID]),
            static_cast<Aircraft::IdType>(Parse<x32>(match[RE_APRS_TYPE]) & 0x03),
            static_cast<Aircraft::AircraftType>((Parse<x32>(match[RE_APRS_TYPE]) & 0x7C) >> 2)};
}

Aircraft::Movement AprsParser::parseMovement(std::cmatch const& match) const
{
    // This needs to be split later to parse independently.
    return {Parse<f64>(match[RE_APRS_HEAD]), Parse<f64>(match[RE_APRS_GND_SPD]) * math::KTS_2_MS,
            std::max(-10000.0, std::min(10000.0, Parse<f64>(match[RE_APRS_CR]) * math::FPM_2_MS))};
}

Timestamp AprsParser::parseTimeStamp(std::cmatch const& match) const
{
    return Timestamp(ToStrView(match[RE_APRS_TIME]));
}
}  // namespace vfrb::feed::parser

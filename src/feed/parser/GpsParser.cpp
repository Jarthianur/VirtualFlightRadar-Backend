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

#include "math/math.hpp"
#include "object/GpsPosition.h"
#include "object/Timestamp.h"
#include "util/string_utils.hpp"

using namespace vfrb::object;

namespace vfrb::feed::parser
{
CGpsParser::CGpsParser()
    : m_gpggaRe("^\\$[A-Z]{2}GGA,(\\d{6}),"    // time
                "(\\d{4}\\.\\d{3,4}),([NS]),"  // latitude, direction
                "(\\d{5}\\.\\d{3,4}),([EW]),"  // longitude, direction
                "(\\d),(\\d{2}),"              // fix quality, number of satellites
                "(\\d+(?:\\.\\d+)?),"          // dilution
                "(\\d+(?:\\.\\d+)?),M,"        // altitude
                "(\\d+(?:\\.\\d+)?)"           // geoid
                ",M,,\\*[0-9A-F]{2}\\s*?$",
                std::regex::optimize | std::regex::icase)
{}

CGpsPosition CGpsParser::Parse(Str&& str_, u32 prio_) const
{
    try
    {
        if (std::cmatch match; str_util::MatchChecksum({str_.c_str(), str_.length()}) &&
                               std::regex_match(str_.c_str(), match, m_gpggaRe))
        {
            return parsePosition(match, prio_);
        }
    }
    catch ([[maybe_unused]] str_util::error::CConversionError const&)
    {}
    catch ([[maybe_unused]] object::error::CTimestampParseError const&)
    {}
    throw error::CParseError();
}

CGpsPosition CGpsParser::parsePosition(std::cmatch const& match_, u32 prio_) const
{
    auto latitude = math::DmToDeg(str_util::Parse<f64>(match_[RE_GGA_LAT]));
    if (match_[RE_GGA_LAT_DIR] == "S")
    {
        latitude = -latitude;
    }
    auto longitude = math::DmToDeg(str_util::Parse<f64>(match_[RE_GGA_LON]));
    if (match_[RE_GGA_LON_DIR] == "W")
    {
        longitude = -longitude;
    }
    auto altitude = math::DoubleToInt(str_util::Parse<f64>(match_[RE_GGA_ALT]));
    return {prio_,
            {latitude, longitude, altitude},
            str_util::Parse<f64>(match_[RE_GGA_GEOID]),
            str_util::Parse<f64>(match_[RE_GGA_DIL]),
            str_util::Parse<u8>(match_[RE_GGA_SAT]),
            str_util::Parse<s8>(match_[RE_GGA_FIX]),
            CTimestamp(str_util::AsStrView(match_[RE_GGA_TIME]))};
}
}  // namespace vfrb::feed::parser

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

#pragma once

#include <regex>

#include "object/CGpsPosition.hpp"

#include "IParser.hpp"

namespace vfrb::feed::parser
{
/**
 * @brief Implement Parser for GPS NMEA sentences.
 */
class CGpsParser : public IParser<object::CGpsPosition>
{
    inline static constexpr auto const RE_GGA_TIME = 1;  ///< GGA regex capture group of time
    inline static constexpr auto const RE_GGA_LAT  = 2;  ///< GGA regex capture group of latitude
    inline static constexpr auto const RE_GGA_LAT_DIR =
        3;                                              ///< GGA regex capture group of latitude orientation
    inline static constexpr auto const RE_GGA_LON = 4;  ///< GGA regex capture group of longitude
    inline static constexpr auto const RE_GGA_LON_DIR =
        5;  ///< GGA regex capture group of longitude orientation
    inline static constexpr auto const RE_GGA_FIX   = 6;   ///< GGA regex capture group of fix quality
    inline static constexpr auto const RE_GGA_SAT   = 7;   ///< GGA regex capture group of sitallite count
    inline static constexpr auto const RE_GGA_DIL   = 8;   ///< GGA regex capture group of dilution
    inline static constexpr auto const RE_GGA_ALT   = 9;   ///< GGA regex capture group of altitude
    inline static constexpr auto const RE_GGA_GEOID = 10;  ///< GGA regex capture group of geoid separation

    std::regex const m_gpggaRe;  ///< Regular expression to parse GGA

    /**
     * @throw vfrb::str_util::error::CConversionError
     * @throw vfrb::object::error::CTimestampParseError
     */
    object::CGpsPosition parsePosition(std::cmatch const& match_, u32 prio_) const;

public:
    CGpsParser();

    object::CGpsPosition Parse(String&& str_, u32 prio_) const override;
};
}  // namespace vfrb::feed::parser

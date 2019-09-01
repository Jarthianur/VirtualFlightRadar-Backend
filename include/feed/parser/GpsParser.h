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

#include <boost/regex.hpp>

#include "object/GpsPosition.h"

#include "Parser.hpp"

namespace vfrb::feed::parser
{
/**
 * @brief Implement Parser for GPS NMEA sentences.
 */
class GpsParser : public Parser<object::GpsPosition>
{
    inline static constexpr auto RE_GGA_TIME = 1;  ///< GGA regex match capture group of time
    inline static constexpr auto RE_GGA_LAT  = 2;  ///< GGA regex match capture group of latitude
    inline static constexpr auto RE_GGA_LAT_DIR =
        3;                                        ///< GGA regex match capture group of latitude orientation
    inline static constexpr auto RE_GGA_LON = 4;  ///< GGA regex match capture group of longitude
    inline static constexpr auto RE_GGA_LON_DIR =
        5;  ///< GGA regex match capture group of longitude orientation
    inline static constexpr auto RE_GGA_FIX   = 6;   ///< GGA regex match capture group of fix quality
    inline static constexpr auto RE_GGA_SAT   = 7;   ///< GGA regex match capture group of sitallite count
    inline static constexpr auto RE_GGA_DIL   = 8;   ///< GGA regex match capture group of dilution
    inline static constexpr auto RE_GGA_ALT   = 9;   ///< GGA regex match capture group of altitude
    inline static constexpr auto RE_GGA_GEOID = 10;  ///< GGA regex match capture group of geoid separation

    static boost::regex const s_GPGGA_RE;  ///< Regular expression to parse GGA

    /**
     * @brief Parse a Position.
     * @param match    The regex match
     * @param position The target position
     * @return true on success, else false
     */
    object::GpsPosition parsePosition(boost::smatch const& match, u32 priority) const;

public:
    GpsParser()                    = default;
    ~GpsParser() noexcept override = default;

    /**
     * @brief Unpack into GpsPosition.
     * @param sentence The string to unpack
     * @param position The position to unpack into
     * @return true on success, else false
     */
    object::GpsPosition unpack(str&& sentence, u32 priority) const override;
};
}  // namespace vfrb::feed::parser

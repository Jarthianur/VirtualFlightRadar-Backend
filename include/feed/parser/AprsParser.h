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

#pragma once

#include <regex>
#include <tuple>

#include "object/Aircraft.h"

#include "Parser.hpp"

namespace vfrb::feed::parser
{
/**
 * @brief Implement Parser for APRS sentences.
 */
class AprsParser : public Parser<object::Aircraft>
{
    using AircraftInfo =
        std::tuple<std::string_view, object::Aircraft::IdType, object::Aircraft::AircraftType>;

    inline static constexpr auto RE_APRS_TIME    = 1;   ///< APRS regex match group of time
    inline static constexpr auto RE_APRS_LAT     = 2;   ///< APRS regex match group of latitude
    inline static constexpr auto RE_APRS_LAT_DIR = 3;   ///< APRS regex match group of latitude orientation
    inline static constexpr auto RE_APRS_LON     = 4;   ///< APRS regex match group of longitude
    inline static constexpr auto RE_APRS_LON_DIR = 5;   ///< APRS regex match group of longitude orientation
    inline static constexpr auto RE_APRS_HEAD    = 6;   ///< APRS regex match group of heading
    inline static constexpr auto RE_APRS_GND_SPD = 7;   ///< APRS regex match group of ground speed
    inline static constexpr auto RE_APRS_ALT     = 8;   ///< APRS regex match group of altitude
    inline static constexpr auto RE_APRS_TYPE    = 9;   ///< APRS regex match group of id and aircraft type
    inline static constexpr auto RE_APRS_ID      = 10;  ///< APRS regex match group of aircraft id
    inline static constexpr auto RE_APRS_CR      = 11;  ///< APRS regex match group of climb rate
    inline static constexpr auto RE_APRS_TR      = 12;  ///< APRS regex match group of turn rate

    std::regex const m_APRS_RE;    ///< Regular expression for APRS protocol
    s32 const        m_maxHeight;  ///< The max height filter

    /**
     * @brief Parse a Position.
     * @param match    The regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    object::Location parseLocation(std::cmatch const& match) const;

    /**
     * @brief Parse the APRS comment.
     * @param match    The regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    AircraftInfo parseComment(std::cmatch const& match) const;

    /**
     * @brief Parse the Movement information.
     * @param match    The regex match
     * @param comMatch The comment regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    object::Aircraft::Movement parseMovement(std::cmatch const& match) const;

    /**
     * @brief Parse the Timestamp information.
     * @param match    The regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    object::Timestamp parseTimeStamp(std::cmatch const& match) const;

public:
    explicit AprsParser(s32 maxHeight);
    ~AprsParser() noexcept override = default;

    /**
     * @brief Unpack into Aircraft.
     * @param sentence The string to unpack
     * @param aircraft The Aircraft to unpack into
     * @return true on success, else false
     */
    object::Aircraft unpack(str&& sentence, u32 priority) const override;
};
}  // namespace vfrb::feed::parser

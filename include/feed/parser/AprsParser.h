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

#include <tuple>

#include <boost/regex.hpp>

#include "object/Aircraft.h"

#include "Parser.hpp"

namespace vfrb::feed::parser
{
/**
 * @brief Implement Parser for APRS sentences.
 */
class AprsParser : public Parser<object::Aircraft>
{
    using AircraftInfo = std::tuple<str, object::Aircraft::IdType, object::Aircraft::AircraftType>;

    inline static constexpr auto RE_APRS_TIME     = 1;  ///< APRS regex match group of time
    inline static constexpr auto RE_APRS_LAT      = 2;  ///< APRS regex match group of latitude
    inline static constexpr auto RE_APRS_LAT_DIR  = 3;  ///< APRS regex match group of latitude orientation
    inline static constexpr auto RE_APRS_LON      = 4;  ///< APRS regex match group of longitude
    inline static constexpr auto RE_APRS_LON_DIR  = 5;  ///< APRS regex match group of longitude orientation
    inline static constexpr auto RE_APRS_HEAD     = 6;  ///< APRS regex match group of heading
    inline static constexpr auto RE_APRS_GND_SPD  = 7;  ///< APRS regex match group of ground speed
    inline static constexpr auto RE_APRS_ALT      = 8;  ///< APRS regex match group of altitude
    inline static constexpr auto RE_APRS_COM      = 9;  ///< APRS regex match group of comment
    inline static constexpr auto RE_APRS_COM_TYPE = 1;  ///< APRS regex match group of id and aircraft type
    inline static constexpr auto RE_APRS_COM_ID   = 2;  ///< APRS regex match group of aircraft id
    inline static constexpr auto RE_APRS_COM_CR   = 3;  ///< APRS regex match group of climb rate
    inline static constexpr auto RE_APRS_COM_TR   = 4;  ///< APRS regex match group of turn rate

    static boost::regex const s_APRS_RE;    ///< Regular expression for APRS protocol
    static boost::regex const s_APRSExtRE;  ///< Regular expression for OGN specific APRS extension

    /**
     * @brief Parse a Position.
     * @param match    The regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    object::Location parseLocation(boost::smatch const& match) const;

    /**
     * @brief Parse the APRS comment.
     * @param match    The regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    AircraftInfo parseComment(boost::smatch const& match) const;

    /**
     * @brief Parse the Movement information.
     * @param match    The regex match
     * @param comMatch The comment regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    object::Aircraft::Movement parseMovement(boost::smatch const& match, boost::smatch const& comMatch) const;

    /**
     * @brief Parse the Timestamp information.
     * @param match    The regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    object::Timestamp<object::time::DateTimeImplBoost> parseTimeStamp(boost::smatch const& match) const;

public:
    static s32 s_maxHeight;  ///< The max height filter

    AprsParser();
    ~AprsParser() noexcept override = default;

    /**
     * @brief Unpack into Aircraft.
     * @param sentence The string to unpack
     * @param aircraft The Aircraft to unpack into
     * @return true on success, else false
     */
    object::Aircraft unpack(str const& sentence, u32 priority) const override;
};
}  // namespace vfrb::feed::parser

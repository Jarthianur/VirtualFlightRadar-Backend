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

#include <cstdint>
#include <string>

#include <boost/regex.hpp>

#include "Parser.hpp"

namespace object
{
class Aircraft;
}  // namespace object

namespace feed
{
namespace parser
{
/**
 * @brief Implement Parser for APRS sentences.
 */
class AprsParser : public Parser<object::Aircraft>
{
    //< begin constants >//
    static constexpr auto RE_APRS_TIME     = 1;  ///< APRS regex match group of time
    static constexpr auto RE_APRS_LAT      = 2;  ///< APRS regex match group of latitude
    static constexpr auto RE_APRS_LAT_DIR  = 3;  ///< APRS regex match group of latitude orientation
    static constexpr auto RE_APRS_LON      = 4;  ///< APRS regex match group of longitude
    static constexpr auto RE_APRS_LON_DIR  = 5;  ///< APRS regex match group of longitude orientation
    static constexpr auto RE_APRS_HEAD     = 6;  ///< APRS regex match group of heading
    static constexpr auto RE_APRS_GND_SPD  = 7;  ///< APRS regex match group of ground speed
    static constexpr auto RE_APRS_ALT      = 8;  ///< APRS regex match group of altitude
    static constexpr auto RE_APRS_COM      = 9;  ///< APRS regex match group of comment
    static constexpr auto RE_APRS_COM_TYPE = 1;  ///< APRS regex match group of id and aircraft type
    static constexpr auto RE_APRS_COM_ID   = 2;  ///< APRS regex match group of aircraft id
    static constexpr auto RE_APRS_COM_CR   = 3;  ///< APRS regex match group of climb rate
    static constexpr auto RE_APRS_COM_TR   = 4;  ///< APRS regex match group of turn rate
    //< end constants >//

    //< begin members >//
    static const boost::regex s_APRS_RE;    ///< Regular expression for APRS protocol
    static const boost::regex s_APRSExtRE;  ///< Regular expression for OGN specific APRS extension
    //< end members >//

    //< begin methods >//
    /**
     * @brief Parse a Position.
     * @param match    The regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    bool parsePosition(const boost::smatch& match, object::Aircraft& aircraft) noexcept;

    /**
     * @brief Parse the APRS comment.
     * @param match    The regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    bool parseComment(const boost::smatch& match, object::Aircraft& aircraft) noexcept;

    /**
     * @brief Parse the Movement information.
     * @param match    The regex match
     * @param comMatch The comment regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    bool parseMovement(const boost::smatch& match, const boost::smatch& comMatch,
                       object::Aircraft& aircraft) noexcept;

    /**
     * @brief Parse the TimeStamp information.
     * @param match    The regex match
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    bool parseTimeStamp(const boost::smatch& match, object::Aircraft& aircraft) noexcept;
    //< end methods >//

public:
    //< begin members >//
    static std::int32_t s_maxHeight;  ///< The max height filter
    //< end members >//

    AprsParser();
    ~AprsParser() noexcept override = default;

    //< begin interfaces >//
    /**
     * @brief Unpack into Aircraft.
     * @param sentence The string to unpack
     * @param aircraft The Aircraft to unpack into
     * @return true on success, else false
     */
    bool unpack(const std::string& sentence, object::Aircraft& aircraft) noexcept override;
    //< end interfaces >//
};
}  // namespace parser
}  // namespace feed

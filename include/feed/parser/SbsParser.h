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

#include "Parser.hpp"

namespace object
{
class Aircraft;
struct Location;
}  // namespace object

namespace feed
{
namespace parser
{
/**
 * @brief Implement Parser for SBS sentences.
 */
class SbsParser : public Parser<object::Aircraft>
{
    //< begin constants >//
    static constexpr auto SBS_FIELD_ID   = 4;   ///< Field number of aircraft id
    static constexpr auto SBS_FIELD_TIME = 7;   ///< Field number of time
    static constexpr auto SBS_FIELD_ALT  = 11;  ///< Field number of altitude
    static constexpr auto SBS_FIELD_LAT  = 14;  ///< Field number of latitude
    static constexpr auto SBS_FIELD_LON  = 15;  ///< Field number of longitude
    //< end constants >//

    //< begin methods >//
    /**
     * @brief Parse a field in SBS and set respective values.
     * @param fieldNr  The field number
     * @param field    The string in that field
     * @param position The target position
     * @param aircraft The target Aircraft
     * @return true on success, else false
     */
    bool parseField(std::uint32_t fieldNr, const std::string& field, object::Location& position,
                    object::Aircraft& aircraft) noexcept;
    //< end methods >//

public:
    //< begin members >//
    static std::int32_t s_maxHeight;  ///< The max height filter
    //< end members >//

    SbsParser();
    ~SbsParser() noexcept override = default;

    //< begin interfaces >//
    /**
     * @brief Unpack into Aircraft.
     * @param sentence The string to unpack
     * @param aircraft The Aircraft to unpack into
     */
    bool unpack(const std::string& sentence, object::Aircraft& aircraft) noexcept override;
    //< end interfaces >//
};
}  // namespace parser
}  // namespace feed

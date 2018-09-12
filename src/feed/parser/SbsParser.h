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

#include "../../Defines.h"
#include "../../object/Aircraft.h"

#include "Parser.hpp"

/// @namespace feed
namespace feed
{
/// @namespace parser
namespace parser
{
/**
 * @class SbsParser
 * @brief Unpack SBS sentences.
 * @implements Parser
 */
class SbsParser : public Parser<object::Aircraft>
{
public:
    SbsParser();

    ~SbsParser() noexcept;

    /**
     * @fn unpack
     * @brief Unpack into Aircraft.
     * @see Parser#unpack
     */
    bool unpack(const std::string& crStr, object::Aircraft& rAircraft) noexcept override;

private:
    /**
     * @fn parseField
     * @brief Parse a field in SBS and set respective values.
     * @param vField    The field number
     * @param crStr     The field string
     * @param rPosition The target position
     * @param rAircraft The target aircraft
     * @return true on success, else false
     */
    bool parseField(std::uint32_t vField, const std::string& crStr, object::Position& rPosition,
                    object::Aircraft& rAircraft) noexcept;

    /// @var mMaxHeight
    /// The max height filter
    static std::int32_t smMaxHeight;

public:
    SETTER_V(smMaxHeight, MaxHeight)
};
}  // namespace parser
}  // namespace feed

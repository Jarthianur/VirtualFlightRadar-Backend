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

#include "object/Atmosphere.h"

#include "Parser.hpp"

namespace feed
{
namespace parser
{
/**
 * @brief Implement Parser for NMEA sentences from sensors.
 */
class AtmosphereParser : public Parser<object::Atmosphere>
{
public:
    AtmosphereParser();
    ~AtmosphereParser() noexcept override = default;

    /**
     * @brief Unpack into Atmosphere.
     * @param sentence   The string to unpack
     * @param atmosphere The Atmosphere to unpack into
     */
    object::Atmosphere unpack(const std::string& sentence, std::uint32_t priority) const override;
};
}  // namespace parser
}  // namespace feed

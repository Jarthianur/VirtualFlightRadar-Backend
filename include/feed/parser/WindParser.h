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

#include <string>

#include "feed/parser/Parser.hpp"
#include "object/Wind.h"
#include "util/defines.h"

namespace feed
{
namespace parser
{
/**
 * @brief Implement Parser for NMEA wind sentences.
 */
class WindParser : public Parser<object::Wind>
{
public:
    DEFAULT_DTOR(WindParser)

    WindParser();

    /**
     * @brief Unpack into Wind.
     * @param sentence The string to unpack
     * @param wind     The Wind to unpack into
     * @return true on success, else false
     */
    bool unpack(const std::string& sentence, object::Wind& wind) noexcept override;
};
}  // namespace parser
}  // namespace feed

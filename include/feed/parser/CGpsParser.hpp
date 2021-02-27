/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <regex>

#include "object/CGpsPosition.hpp"

#include "IParser.hpp"

namespace vfrb::feed::parser
{
class CGpsParser : public IParser<object::CGpsPosition>
{
    CTCONST RE_GGA_TIME    = 1;
    CTCONST RE_GGA_LAT     = 2;
    CTCONST RE_GGA_LAT_DIR = 3;
    CTCONST RE_GGA_LON     = 4;
    CTCONST RE_GGA_LON_DIR = 5;
    CTCONST RE_GGA_FIX     = 6;
    CTCONST RE_GGA_SAT     = 7;
    CTCONST RE_GGA_DIL     = 8;
    CTCONST RE_GGA_ALT     = 9;
    CTCONST RE_GGA_GEOID   = 10;

    std::regex const m_gpggaRe;

    [[nodiscard]] auto static parsePosition(std::cmatch const& match_, u32 prio_) -> object::CGpsPosition;

public:
    CGpsParser();

    auto
    Parse(String&& str_, u32 prio_) const -> object::CGpsPosition override;
};
}  // namespace vfrb::feed::parser

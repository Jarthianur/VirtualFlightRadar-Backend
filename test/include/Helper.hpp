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

#include <cstdint>
#include <iostream>
#include <string>
#include <tpp.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "object/CAircraft.hpp"

#define SYSO(M) std::cout << M << std::flush

using tpp::operator""_re;

namespace tpp::intern
{
template<>
inline auto
to_string<vfrb::object::CAircraft::ETargetType>(vfrb::object::CAircraft::ETargetType const& arg_)
    -> std::string {
    return std::to_string(vfrb::AsUnderlyingType(arg_));
}
}  // namespace tpp::intern

namespace helper
{
static auto const PflauRE = R"(\$PFLAU,,,,1,0,([-]?\d+?),0,(\d+?),(\d+?),(\S{6})\*(?:\S{2}))"_re;
static auto const PflaaRE =
    R"(\$PFLAA,0,([-]?\d+?),([-]?\d+?),([-]?\d+?),(\d+?),(\S{6}),(\d{3})?,,(\d+?)?,([-]?\d+?\.\d+?)?,([0-9A-F]{1,2})\*(?:\S{2}))"_re;
static auto const GpsRE =
    R"((\$GPRMC,\d{6},A,0000\.00,N,00000\.00,E,0,0,\d{6},001\.0,W\*[0-9a-fA-F]{2}\s*)?(\$GPGGA,\d{6},0000\.0000,N,00000\.0000,E,1,00,1,0,M,0\.0,M,,\*[0-9a-fA-F]{2}\s*)?)"_re;
}  // namespace helper

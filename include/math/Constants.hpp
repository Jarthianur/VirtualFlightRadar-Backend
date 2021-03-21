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

#include <cmath>

#include "util/ClassUtils.hpp"

namespace vfrb::math
{
CTCONST           KTS_2_KMH = 1.852;
CTCONST           KMH_2_KTS = 0.539957;
CTCONST           KTS_2_MS  = 0.514444;
CTCONST           MS_2_KTS  = 1.94384;
CTCONST           KMH_2_MS  = 0.277778;
CTCONST           MS_2_KMH  = 3.6;
CTCONST           MS_2_FPM  = 196.85;
CTCONST           FPM_2_MS  = 0.00508;
CTCONST           FEET_2_M  = 0.3048;
CTCONST           M_2_FEET  = 3.28084;
static auto const PI        = std::acos(-1.0);
}  // namespace vfrb::math

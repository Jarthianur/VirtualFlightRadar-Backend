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

#include <cmath>

#include "util/class_utils.hpp"

namespace vfrb::math
{
CTCONST           KTS_2_KMH = 1.852;            ///< Convert knots to km/h
CTCONST           KMH_2_KTS = 0.539957;         ///< Convert km/h to knots
CTCONST           KTS_2_MS  = 0.514444;         ///< Convert knots to m/s
CTCONST           MS_2_KTS  = 1.94384;          ///< Convert m/s to knots
CTCONST           KMH_2_MS  = 0.277778;         ///< Convert km/h to m/s
CTCONST           MS_2_KMH  = 3.6;              ///< Convert m/s to km/h
CTCONST           MS_2_FPM  = 196.85;           ///< Convert m/s to fpm
CTCONST           FPM_2_MS  = 0.00508;          ///< Convert fpm to m/s
CTCONST           FEET_2_M  = 0.3048;           ///< Convert feet to m
CTCONST           M_2_FEET  = 3.28084;          ///< Convert m to feet
static auto const PI        = std::acos(-1.0);  ///< The circular number
}  // namespace vfrb::math

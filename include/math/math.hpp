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

#include <algorithm>
#include <cmath>

#include "constants.hpp"
#include "types.hpp"

using namespace std::literals;

namespace vfrb::math
{
/**
 * Convert degrees to radian.
 * @param deg_ The degrees
 * @return the radian
 */
inline f64 Radian(f64 deg_) {
    return ((deg_ * PI) / 180.0);
}

/**
 * Convert radian to degrees.
 * @param radian The radian
 * @return the degrees
 */
inline f64 Degree(f64 rad_) {
    return (rad_ * 180.0) / PI;
}

/**
 * Convert floating point to integer, round to nearest.
 * @param value The floating point value
 * @return the rounded integer
 */
inline s32 DoubleToInt(f64 val_) {
    return (val_ >= 0.0) ? static_cast<s32>(val_ + 0.5) : static_cast<s32>(val_ - 0.5);
}

/**
 * Convert ( degree, minute-as-decimal ) to degree.
 * @param degMin_ The degree-minute value
 * @return the degree value
 */
inline f64 DmToDeg(f64 degMin_) {
    f64 absDm = std::abs(degMin_ / 100.0);
    f64 d     = std::floor(absDm);
    f64 m     = (absDm - d) * 100.0 / 60.0;
    return d + m;
}

/**
 * Saturate a value to given bounds.
 * @param val_ The value to saturate
 * @param min_ The lower bound
 * @param max_ The upper bound
 * @return the saturated value
 */
template<typename T>
T Saturate(T val_, T min_, T max_) {
    return std::max(min_, std::min(val_, max_));
}

/**
 * Calculate height difference from QNE to pressure in meters with ICAO height formula.
 * @param press_ The air pressure
 * @return the height difference to QNE
 */
inline s32 IcaoHeight(f64 press_) {
    return DoubleToInt(288.15 * (1.0 - std::pow((press_ / 1013.25), 0.190295)) / 0.0065);
}
}  // namespace vfrb::math

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

#include "Constants.hpp"
#include "Types.hpp"

namespace vfrb::math
{
inline auto
Radian(f64 deg_) -> f64 {
    return ((deg_ * PI) / 180.0);
}

inline auto
Degree(f64 rad_) -> f64 {
    return (rad_ * 180.0) / PI;
}

inline auto
DoubleToInt(f64 val_) -> i32 {
    return static_cast<i32>(std::lround(val_));
}

inline auto
DmToDeg(f64 degMin_) -> f64 {
    f64 absDm = std::abs(degMin_ / 100.0);
    f64 d     = std::floor(absDm);
    f64 m     = (absDm - d) * 100.0 / 60.0;
    return d + m;
}

template<typename T>
auto
Saturate(T val_, T min_, T max_) -> T {
    return std::max(min_, std::min(val_, max_));
}

inline auto
IcaoHeight(f64 press_) -> i32 {
    return DoubleToInt(288.15 * (1.0 - std::pow((press_ / 1013.25), 0.190295)) / 0.0065);
}
}  // namespace vfrb::math

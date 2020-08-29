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

#include <type_traits>

namespace vfrb::util
{
/**
 * Get an enum value as the underlying type.
 * @tparam T The enum type
 * @param value The enum value
 * @return the value as its underlyig type
 */
template<typename T>
constexpr auto
AsUnderlyingType(T val_) -> typename std::underlying_type<T>::type {
    return static_cast<typename std::underlying_type<T>::type>(val_);
}
}  // namespace vfrb::util

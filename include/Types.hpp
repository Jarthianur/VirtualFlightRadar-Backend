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

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace vfrb
{
using u8    = std::uint8_t;
using s8    = std::int8_t;
using u16   = std::uint16_t;
using s16   = std::int16_t;
using u32   = std::uint32_t;
using s32   = std::int32_t;
using u64   = std::uint64_t;
using s64   = std::int64_t;
using usize = std::size_t;
using f32   = float;
using f64   = double;

using enum_type = std::uint_fast8_t;

using str        = char const*;
using String     = std::string;
using StringView = std::string_view;

template<typename T>
using SPtr = std::shared_ptr<T>;

template<typename T>
using UPtr = std::unique_ptr<T>;
}  // namespace vfrb

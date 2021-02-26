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
#include <limits>
#include <list>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace vfrb
{
using u8    = std::uint8_t;
using i8    = std::int8_t;
using u16   = std::uint16_t;
using i16   = std::int16_t;
using u32   = std::uint32_t;
using i32   = std::int32_t;
using u64   = std::uint64_t;
using i64   = std::int64_t;
using usize = std::size_t;
using f32   = float;
using f64   = double;

namespace limits
{
using u8    = std::numeric_limits<u8>;
using i8    = std::numeric_limits<i8>;
using u16   = std::numeric_limits<u16>;
using i16   = std::numeric_limits<i16>;
using u32   = std::numeric_limits<u32>;
using i32   = std::numeric_limits<i32>;
using u64   = std::numeric_limits<u64>;
using i64   = std::numeric_limits<i64>;
using usize = std::numeric_limits<usize>;
using f32   = std::numeric_limits<f32>;
using f64   = std::numeric_limits<f64>;
}  // namespace limits

using enum_type = std::uint_fast8_t;

using str        = char const*;
using String     = std::string;
using StringView = std::string_view;

template<typename T>
using Vector = std::vector<T>;

template<typename T>
using List = std::list<T>;

template<typename K, typename V>
using HashMap = std::unordered_map<K, V>;

template<typename... T>
using Tuple = std::tuple<T...>;

template<typename T>
using SPtr = std::shared_ptr<T>;

template<typename T>
using UPtr = std::unique_ptr<T>;

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

enum class EErrc : enum_type
{
    OK,
    ERR
};

template<typename T>
struct Result
{
    T           Value;
    EErrc const Ec;

    operator bool() const {
        return Ec == EErrc::OK;
    }
};

template<>
struct Result<void>
{
    EErrc Ec;

    operator bool() const {
        return Ec == EErrc::OK;
    }
};

template<typename T>
auto
Ok(T t_) -> Result<T> {
    return {std::move(t_), EErrc::OK};
}

inline auto
Ok() -> Result<void> {
    return {EErrc::OK};
}

template<typename T>
auto
Err(T t_) -> Result<T> {
    return {std::move(t_), EErrc::ERR};
}

inline auto
Err() -> Result<void> {
    return {EErrc::ERR};
}
}  // namespace vfrb

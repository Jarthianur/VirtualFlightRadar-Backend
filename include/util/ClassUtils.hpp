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
#include <utility>

#define NOT_COPYABLE(TYPE)      \
    TYPE(TYPE const&) = delete; \
    auto operator=(TYPE const&)->TYPE& = delete;

#define NOT_MOVABLE(TYPE)  \
    TYPE(TYPE&&) = delete; \
    auto operator=(TYPE&&) noexcept->TYPE& = delete;

#define MOVABLE(TYPE)      \
    TYPE(TYPE&&) noexcept; \
    auto operator=(TYPE&&) noexcept->TYPE&;

#define COPYABLE(TYPE) \
    TYPE(TYPE const&); \
    auto operator=(TYPE const&)->TYPE&;

#define DEFAULT_COPYABLE(TYPE)   \
    TYPE(TYPE const&) = default; \
    auto operator=(TYPE const&)->TYPE& = default;

#define COPY_ON_MOVE(TYPE)                      \
    TYPE(TYPE&& o_) noexcept : TYPE(o_){};      \
    auto operator=(TYPE&& o_) noexcept->TYPE& { \
        this->operator=(o_);                    \
        return *this;                           \
    };

#define CTCONST inline static constexpr auto const

#define GETTER(FN, MEM)                     \
    inline auto FN() const->decltype(MEM) { \
        return MEM;                         \
    }

#define GETTER_REF(FN, MEM)                        \
    inline auto FN() const->decltype(MEM) const& { \
        return MEM;                                \
    }

#define FUNCTION_ALIAS(ALIAS, FN)                                                     \
    template<typename T, typename... Args>                                            \
    inline auto ALIAS(Args&&... args)->decltype(FN<T>(std::forward<Args>(args)...)) { \
        return FN<T>(std::forward<Args>(args)...);                                    \
    }

#define ENABLE_IF(C) typename std::enable_if<C>::type* = nullptr

#define AND &&

#define OR ||

#define NOT !

#define IS(C, ...) (C<__VA_ARGS__>::value)

#define IS_TYPE(T, R) (IS(std::is_same, T, R))

#define EXTENDS(T, R) (IS(std::is_base_of, R, T))

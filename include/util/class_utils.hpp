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

/**
 * Make a class non copyable.
 * @param TYPE The classname
 */
#define NOT_COPYABLE(TYPE)                 \
    TYPE(TYPE const&) = delete;            \
    TYPE& operator=(TYPE const&) = delete; \
    TYPE(TYPE&&)                 = delete; \
    TYPE& operator=(TYPE&&) noexcept = delete;

/**
 * Make a class non copyable, but movable.
 * @param TYPE The classname
 */
#define MOVABLE_BUT_NOT_COPYABLE(TYPE)     \
    TYPE(TYPE const&) = delete;            \
    TYPE& operator=(TYPE const&) = delete; \
    TYPE(TYPE&&) noexcept;                 \
    TYPE& operator=(TYPE&&) noexcept;

#define COPYABLE_BUT_NOT_MOVABLE(TYPE)      \
    TYPE(TYPE const&) = default;            \
    TYPE& operator=(TYPE const&) = default; \
    TYPE(TYPE&&) noexcept        = delete;  \
    TYPE& operator=(TYPE&&) noexcept = delete;

#define CTCONST inline static constexpr auto const

#define GETTER(FN, MEM)                     \
    inline auto FN() const->decltype(MEM) { \
        return MEM;                         \
    }

#define GETTER_REF(FN, MEM)                        \
    inline auto FN() const->decltype(MEM) const& { \
        return MEM;                                \
    }

/**
 * Define an alias for a function. This is useful for keeping compatibility to STL interfaces.
 * @param ALIAS The alias name
 * @param FN    The original function
 */
#define FUNCTION_ALIAS(ALIAS, FN)                                                  \
    template<typename... Args>                                                     \
    inline auto ALIAS(Args&&... args)->decltype(FN(std::forward<Args>(args)...)) { \
        return FN(std::forward<Args>(args)...);                                    \
    }

/**
 * Wrapper for template enabling expression.
 * @param C The condition to enable on
 */
#define ENABLE_IF(C) typename std::enable_if<C>::type* = nullptr

/// Logical and
#define AND &&

/// Logical or
#define OR ||

/// Invert
#define NOT !

/**
 * Template constraint
 * @param C   The constraint
 * @param ... The contraints args
 */
#define IS(C, ...) (C<__VA_ARGS__>::value)

/**
 * Template constraint for same type
 * @param T The type to check
 * @param R The required type
 */
#define IS_TYPE(T, R) (IS(std::is_same, T, R))

/**
 * Template constraint for inherited type
 * @param T The type to check
 * @param R The type required to be derived from
 */
#define EXTENDS(T, R) (IS(std::is_base_of, R, T))

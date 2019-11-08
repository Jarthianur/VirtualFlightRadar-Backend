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

#include <type_traits>
#include <utility>

/**
 * @file Defines.h
 *
 * This header contains come common macros, used throughout the software.
 */

/// Make a class of TYPE non copyable.
/// @param TYPE The classname
#define NOT_COPYABLE(TYPE)      \
    TYPE(TYPE const&) = delete; \
    TYPE& operator=(TYPE const&) = delete;

/// Make a class of TYPE non copyable, but movable.
/// @param TYPE The classname
#define MOVABLE_BUT_NOT_COPYABLE(TYPE)     \
    TYPE(TYPE const&) = delete;            \
    TYPE& operator=(TYPE const&) = delete; \
    TYPE(TYPE&&);                          \
    TYPE& operator=(TYPE&&);

#define FUNCTION_ALIAS(ALIAS, FN)                                                \
    template<typename... Args>                                                   \
    inline auto ALIAS(Args&&... args)->decltype(FN(std::forward<Args>(args)...)) \
    {                                                                            \
        return FN(std::forward<Args>(args)...);                                  \
    }

#define ENABLE_IF(C) typename std::enable_if<C>::type* = nullptr
#define AND &&
#define OR ||
#define NOT !
#define IS(T, ...) (T<__VA_ARGS__>::value)
#define IS_TYPE(T, R) (IS(std::is_same, T, R))
#define EXTENDS(T, R) (IS(std::is_base_of, R, T))

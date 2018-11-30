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

/**
 * @file Defines.h
 *
 * This header contains come common macros, used throughout the software.
 */

/// @def GETTER_V
/// @param MEMBER The member
/// Define and declare an inlined by-value-getter.
#define GETTER_V(MEMBER)                                   \
    inline auto get_##MEMBER() const->decltype(m_##MEMBER) \
    {                                                      \
        return m_##MEMBER;                                 \
    }

/// @def GETTER_R
/// @param MEMBER The member
/// Define and declare an inlined by-reference-getter.
#define GETTER_R(MEMBER)                              \
    inline auto get_##MEMBER()->decltype(m_##MEMBER)& \
    {                                                 \
        return m_##MEMBER;                            \
    }

/// @def GETTER_CR
/// @param MEMBER The member
/// Define and declare an inlined by-const_reference-getter.
#define GETTER_CR(MEMBER)                                         \
    inline auto get_##MEMBER() const->const decltype(m_##MEMBER)& \
    {                                                             \
        return m_##MEMBER;                                        \
    }

/// @def SETTER_V
/// @param MEMBER The member
/// Define and declare an inlined by-value-setter.
#define SETTER_V(MEMBER)                               \
    inline void set_##MEMBER(decltype(m_##MEMBER) val) \
    {                                                  \
        m_##MEMBER = val;                              \
    }

/// @def SETTER_CR
/// @param MEMBER The member
/// Define and declare an inlined by-const_reference-setter.
#define SETTER_CR(MEMBER)                                     \
    inline void set_##MEMBER(const decltype(m_##MEMBER)& ref) \
    {                                                         \
        m_##MEMBER = ref;                                     \
    }

/// @def GETSET_V
/// @param MEMBER The member
/// Define and declare inlined by-value-getter/setter.
#define GETSET_V(MEMBER) \
    GETTER_V(MEMBER)     \
    SETTER_V(MEMBER)

/// @def GETSET_CR
/// @param MEMBER The member
/// Define and declare inlined by-const_reference-getter/setter.
#define GETSET_CR(MEMBER) \
    GETTER_CR(MEMBER)     \
    SETTER_CR(MEMBER)

/// @def DEFAULT_CTOR
/// @param TYPE The classname
/// Define default constructor for class.
#define DEFAULT_CTOR(TYPE) TYPE() = default;

/// @def DEFAULT_DTOR
/// @param TYPE The classname
/// Define default destructor for class.
#define DEFAULT_DTOR(TYPE) ~TYPE() noexcept = default;

/// @def DEFAULT_VIRTUAL_DTOR
/// @param TYPE The classname
/// Define virtual default destructor for class.
#define DEFAULT_VIRTUAL_DTOR(TYPE) virtual ~TYPE() noexcept = default;

/// @def NON_COPYABLE
/// @param TYPE The classname
/// Make a class of TYPE non copyable.
#define NOT_COPYABLE(TYPE)      \
    TYPE(const TYPE&) = delete; \
    TYPE& operator=(const TYPE&) = delete;

/// @def MOVABLE_BUT_NOT_COPYABLE
/// @param TYPE The classname
/// Make a class of TYPE non copyable, but movable.
#define MOVABLE_BUT_NOT_COPYABLE(TYPE)     \
    TYPE(const TYPE&) = delete;            \
    TYPE& operator=(const TYPE&) = delete; \
    TYPE(TYPE&&);                          \
    TYPE& operator=(TYPE&&);

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

/// @def GETTER_V(MEMBER, NAME)
/// Define and declare an inlined by-value-getter.
/// NAME is the method suffix.
#define GETTER_V(MEMBER)                             \
    inline decltype(m_##MEMBER) get_##MEMBER() const \
    {                                                \
        return m_##MEMBER;                           \
    }

/// @def GETTER_R(MEMBER, NAME)
/// Define and declare an inlined by-reference-getter.
/// NAME is the method suffix.
#define GETTER_R(MEMBER)                        \
    inline decltype(m_##MEMBER)& get_##MEMBER() \
    {                                           \
        return m_##MEMBER;                      \
    }

/// @def GETTER_CR(MEMBER, NAME)
/// Define and declare an inlined by-const_reference-getter.
/// NAME is the method suffix.
#define GETTER_CR(MEMBER)                                   \
    inline const decltype(m_##MEMBER)& get_##MEMBER() const \
    {                                                       \
        return m_##MEMBER;                                  \
    }

/// @def SETTER_V(MEMBER, NAME)
/// Define and declare an inlined by-value-setter.
/// NAME is the method suffix.
#define SETTER_V(MEMBER)                               \
    inline void set_##MEMBER(decltype(m_##MEMBER) val) \
    {                                                  \
        m_##MEMBER = val;                              \
    }

/// @def SETTER_CR(MEMBER, NAME)
/// Define and declare an inlined by-const_reference-setter.
/// NAME is the method suffix.
#define SETTER_CR(MEMBER)                                     \
    inline void set_##MEMBER(const decltype(m_##MEMBER)& ref) \
    {                                                         \
        m_##MEMBER = ref;                                     \
    }

/// @def GETSET_V(MEMBER, NAME)
/// Define and declare inlined by-value-getter/setter.
/// NAME is the method suffix.
#define GETSET_V(MEMBER) \
    GETTER_V(MEMBER)     \
    SETTER_V(MEMBER)

/// @def GETSET_CR(MEMBER, NAME)
/// Define and declare inlined by-const_reference-getter/setter.
/// NAME is the method suffix.
#define GETSET_CR(MEMBER) \
    GETTER_CR(MEMBER)     \
    SETTER_CR(MEMBER)

/// @def NON_COPYABLE
/// Make a class of TYPE non copyable.
#define NOT_COPYABLE(TYPE)      \
    TYPE(const TYPE&) = delete; \
    TYPE& operator=(const TYPE&) = delete;

/// @def MOVABLE_BUT_NOT_COPYABLE
/// Make a class of TYPE non copyable, but movable.
#define MOVABLE_BUT_NOT_COPYABLE(TYPE)     \
    TYPE(const TYPE&) = delete;            \
    TYPE& operator=(const TYPE&) = delete; \
    TYPE(TYPE&&);                          \
    TYPE& operator=(TYPE&&);

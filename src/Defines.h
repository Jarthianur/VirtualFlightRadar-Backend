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
#define GETTER_V(MEMBER, NAME)                \
    inline decltype(MEMBER) get##NAME() const \
    {                                         \
        return MEMBER;                        \
    }

/// @def GETTER_R(MEMBER, NAME)
/// Define and declare an inlined by-reference-getter.
/// NAME is the method suffix.
#define GETTER_R(MEMBER, NAME)           \
    inline decltype(MEMBER)& get##NAME() \
    {                                    \
        return MEMBER;                   \
    }

/// @def GETTER_CR(MEMBER, NAME)
/// Define and declare an inlined by-const_reference-getter.
/// NAME is the method suffix.
#define GETTER_CR(MEMBER, NAME)                      \
    inline const decltype(MEMBER)& get##NAME() const \
    {                                                \
        return MEMBER;                               \
    }

/// @def SETTER_V(MEMBER, NAME)
/// Define and declare an inlined by-value-setter.
/// NAME is the method suffix.
#define SETTER_V(MEMBER, NAME)                   \
    inline void set##NAME(decltype(MEMBER) vNew) \
    {                                            \
        MEMBER = vNew;                           \
    }

/// @def SETTER_CR(MEMBER, NAME)
/// Define and declare an inlined by-const_reference-setter.
/// NAME is the method suffix.
#define SETTER_CR(MEMBER, NAME)                          \
    inline void set##NAME(const decltype(MEMBER)& crNew) \
    {                                                    \
        MEMBER = crNew;                                  \
    }

/// @def GETSET_V(MEMBER, NAME)
/// Define and declare inlined by-value-getter/setter.
/// NAME is the method suffix.
#define GETSET_V(MEMBER, NAME)                   \
    inline decltype(MEMBER) get##NAME() const    \
    {                                            \
        return MEMBER;                           \
    }                                            \
    inline void set##NAME(decltype(MEMBER) vNew) \
    {                                            \
        MEMBER = vNew;                           \
    }

/// @def GETSET_CR(MEMBER, NAME)
/// Define and declare inlined by-const_reference-getter/setter.
/// NAME is the method suffix.
#define GETSET_CR(MEMBER, NAME)                          \
    inline const decltype(MEMBER)& get##NAME() const     \
    {                                                    \
        return MEMBER;                                   \
    }                                                    \
    inline void set##NAME(const decltype(MEMBER)& crNew) \
    {                                                    \
        MEMBER = crNew;                                  \
    }

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

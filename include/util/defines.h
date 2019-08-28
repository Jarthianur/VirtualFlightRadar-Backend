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

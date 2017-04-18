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

#ifndef SRC_UTIL_PRIORITY_H_
#define SRC_UTIL_PRIORITY_H_

#include <cstdint>

/**
 * Priorities / update policies
 * Override data if not valid, or lower.
 *
 * DONTCARE : used for single input (no other feed for this data),
 *     or fallbacks. Overrides itself, but nothing else.
 * LESSER: used as "I actually want input from other feed, but if not take it."
 *     Overrides only DC, even not itself, until invalid. Data stays invalid.
 * NORMAL: used for common backup feeds. Overrides itself and all but HIGHER.
 * HIGHER: used for main feed. Overrides all, no matter what state.
 */
enum class Priority
    : std::uint32_t
    {
        DONTCARE = 0,
    LESSER = 1,
    NORMAL = 2,
    HIGHER = 3
};

#endif /* SRC_UTIL_PRIORITY_H_ */

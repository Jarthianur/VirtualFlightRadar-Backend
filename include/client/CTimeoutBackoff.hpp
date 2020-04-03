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

#include "types.hpp"

namespace vfrb::client
{
/**
 * @brief The TimeoutBackoff class
 * Produces backoff sequence 1s,5s,20s,1m,2m
 */
class CTimeoutBackoff
{
    inline static constexpr auto const INITIAL_TIMEOUT = 1;
    inline static constexpr auto const INITIAL_FACTOR  = 5;

    u32 m_timeout = INITIAL_TIMEOUT;  ///< seconds
    u32 m_factor  = INITIAL_FACTOR;

public:
    u32  Next();
    void Reset();
};
}  // namespace vfrb::client

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

#include <array>

#include "util/ClassUtils.hpp"

#include "Types.hpp"

namespace vfrb::client
{
/**
 * @brief The TimeoutBackoff class
 * Produces backoff sequence 1s,5s,20s,1m,2m,5m
 */
class CTimeoutBackoff
{
    CTCONST BACKOFF_SIZE = 6;
    CTCONST BACKOFF_LAST = BACKOFF_SIZE - 1;

    usize m_toIndex = 0;

    std::array<u32, BACKOFF_SIZE> const m_backoffValues = {1, 5, 20, 60, 120, 300};

public:
    auto
    Next() -> u32;
    void
    Reset();
};
}  // namespace vfrb::client

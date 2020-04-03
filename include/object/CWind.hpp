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

#include "util/class_utils.hpp"

#include "CObject.hpp"

namespace vfrb::object
{
struct SClimate;

/// Wind information object
class CWind : public CObject
{
    String m_nmea;  ///< The NMEA string

public:
    MOVABLE_BUT_NOT_COPYABLE(CWind)

    CWind() = default;

    /// @param prio_ The initial priority
    CWind(u32 prio_, String&& nmea_);
    ~CWind() noexcept override = default;

    void assign(CObject&& other_) override;

    void Clear();

    /// Get the NMEA string.
    auto Nmea() const -> decltype(m_nmea) const&;
};
}  // namespace vfrb::object

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

#include "util/class_utils.hpp"

#include "CObject.hpp"

namespace vfrb::object
{
struct SClimate;

/// Atmospheric information object
class CAtmosphere : public CObject
{
    CTCONST ICAO_STD     = 1013.25;  ///< ICAO standard atmospheric pressure at MSL
    CTCONST MAX_PRESSURE = 2000.0;
    CTCONST MIN_PRESSURE = 0.0;

    f64    m_pressure = ICAO_STD;  ///< The atmospheric pressure
    String m_nmea;                 ///< The NMEA string

    void assign(CObject&& other_) override;

public:
    MOVABLE(CAtmosphere)
    NOT_COPYABLE(CAtmosphere)

    CAtmosphere() = default;

    /// @param prio_ The initial priority
    CAtmosphere(u32 prio_, String&& nmea_);

    /// @param prio_ The initial priority
    CAtmosphere(u32 prio_, f64 press_, String&& nmea_);
    ~CAtmosphere() noexcept override = default;

    GETTER_REF(Nmea, m_nmea)
    GETTER(Pressure, m_pressure)
};
}  // namespace vfrb::object

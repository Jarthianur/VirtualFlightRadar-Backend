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

#include "object/CAircraft.hpp"
#include "object/CGpsPosition.hpp"
#include "util/ClassUtils.hpp"
#include "util/StringUtils.hpp"

#include "Types.hpp"

namespace vfrb::data::processor
{
class CAircraftProcessor
{
private:
    CTCONST ICAO_STD_ATMOSPHERE = 1013.25;

    i32 const         m_maxDistance;
    object::SLocation m_refLocation{0.0, 0.0, 0};
    f64               m_refAtmPressure = ICAO_STD_ATMOSPHERE;
    f64 mutable m_refRadLatitude       = 0.0;
    f64 mutable m_aircraftRadLatitude  = 0.0;
    f64 mutable m_latDistance          = 0.0;
    f64 mutable m_refRadLongitude      = 0.0;
    f64 mutable m_aircraftRadLongitude = 0.0;
    f64 mutable m_lonDistance          = 0.0;
    f64 mutable m_relBearing           = 0.0;
    f64 mutable m_absBearing           = 0.0;
    i32 mutable m_relNorth             = 0;
    i32 mutable m_relEast              = 0;
    i32 mutable m_relVertical          = 0;
    i32 mutable m_distance             = 0;

    void
    calculateRelPosition(object::CAircraft const& aircraft_) const;

    void
    appendPflau(object::CAircraft const& aircraft_, str_util::StringInserter& nmea_) const;

    void
    appendPflaa(object::CAircraft const& aircraft_, str_util::StringInserter& nmea_) const;

public:
    CAircraftProcessor();
    explicit CAircraftProcessor(i32 maxDist_);

    void
    Process(object::CAircraft const& aircraft_, str_util::StringInserter& nmea_) const;

    void
    ReferTo(object::SLocation const& loc_, f64 press_);
};
}  // namespace vfrb::data::processor

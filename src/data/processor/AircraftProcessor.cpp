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

#include "data/processor/AircraftProcessor.h"

#include <cmath>
#include <cstdio>
#include <limits>

#include "math/math.hpp"
#include "math/constants.h"
#include "util/string_utils.hpp"
#include "util/utility.hpp"

using namespace vfrb::object;
using namespace vfrb::str_util;

namespace vfrb::data::processor
{
CAircraftProcessor::CAircraftProcessor() : CAircraftProcessor(std::numeric_limits<s32>::max()) {}

CAircraftProcessor::CAircraftProcessor(s32 maxDist) : m_maxDistance(maxDist) {}

void CAircraftProcessor::Process(CAircraft const& aircraft, util::CString<NMEA_SIZE>& nmea) const
{
    calculateRelPosition(aircraft);
    if (m_distance <= m_maxDistance)
    {
        appendPflaa(aircraft, nmea, appendPflau(aircraft, nmea, 0));
    }
    else
    {
        nmea.Clear();
    }
}

void CAircraftProcessor::ReferTo(SLocation const& location, f64 atmPress)
{
    m_refLocation    = location;
    m_refAtmPressure = atmPress;
}

void CAircraftProcessor::calculateRelPosition(CAircraft const& aircraft) const
{
    m_refRadLatitude       = math::Radian(m_refLocation.Latitude);
    m_refRadLongitude      = math::Radian(m_refLocation.Longitude);
    m_aircraftRadLongitude = math::Radian(aircraft.Location().longitude);
    m_aircraftRadLatitude  = math::Radian(aircraft.Location().latitude);
    m_lonDistance          = m_aircraftRadLongitude - m_refRadLongitude;
    m_latDistance          = m_aircraftRadLatitude - m_refRadLatitude;

    f64 a = std::pow(std::sin(m_latDistance / 2.0), 2.0) + std::cos(m_refRadLatitude) *
                                                               std::cos(m_aircraftRadLatitude) *
                                                               std::pow(std::sin(m_lonDistance / 2.0), 2.0);
    m_distance = math::DoubleToInt(6371000.0 * (2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a))));

    m_relBearing = math::Degree(
        std::atan2(std::sin(m_aircraftRadLongitude - m_refRadLongitude) * std::cos(m_aircraftRadLatitude),
                   std::cos(m_refRadLatitude) * std::sin(m_aircraftRadLatitude) -
                       std::sin(m_refRadLatitude) * std::cos(m_aircraftRadLatitude) *
                           std::cos(m_aircraftRadLongitude - m_refRadLongitude)));
    m_absBearing = std::fmod((m_relBearing + 360.0), 360.0);

    m_relNorth    = math::DoubleToInt(std::cos(math::Radian(m_absBearing)) * m_distance);
    m_relEast     = math::DoubleToInt(std::sin(math::Radian(m_absBearing)) * m_distance);
    m_relVertical = aircraft.TargetType() == CAircraft::ETargetType::TRANSPONDER ?
                        aircraft.Location().altitude - math::IcaoHeight(m_refAtmPressure) :
                        aircraft.Location().altitude - m_refLocation.Altitude;
}

usize CAircraftProcessor::appendPflau(CAircraft const& aircraft, util::CString<NMEA_SIZE>& nmea,
                                     usize pos) const
{
    int next = nmea.Format(pos, "$PFLAU,,,,1,0,%d,0,%d,%d,%s*", math::DoubleToInt(m_relBearing),
                           m_relVertical, m_distance, (*aircraft.Id()).data());
    next += nmea.Format(pos, "%02x\r\n", Checksum(*nmea, pos));
    return pos + static_cast<usize>(next);
}

usize CAircraftProcessor::appendPflaa(CAircraft const& aircraft, util::CString<NMEA_SIZE>& nmea,
                                     usize pos) const
{
    int next = 0;
    if (aircraft.HasFullInfo())
    {
        next = nmea.Format(
            pos, "$PFLAA,0,%d,%d,%d,%hhu,%s,%.3d,,%d,%3.1lf,%.1hhX*", m_relNorth, m_relEast, m_relVertical,
            util::AsUnderlyingType(aircraft.IdType()), (*aircraft.Id()).data(),
            math::DoubleToInt(math::Saturate(aircraft.Movement().heading, CAircraft::SMovement::MIN_HEADING,
                                             CAircraft::SMovement::MAX_HEADING)),
            math::DoubleToInt(math::Saturate(aircraft.Movement().gndSpeed * math::MS_2_KMH,
                                             CAircraft::SMovement::MIN_GND_SPEED,
                                             CAircraft::SMovement::MAX_GND_SPEED)),
            math::Saturate(aircraft.Movement().climbRate, CAircraft::SMovement::MIN_CLIMB_RATE,
                           CAircraft::SMovement::MAX_CLIMB_RATE),
            util::AsUnderlyingType(aircraft.AircraftType()));
    }
    else
    {
        next = nmea.Format(pos, "$PFLAA,0,%d,%d,%d,1,%s,,,,,%1hhX*", m_relNorth, m_relEast, m_relVertical,
                           (*aircraft.Id()).data(), util::AsUnderlyingType(aircraft.AircraftType()));
    }
    next += nmea.Format(pos, "%02x\r\n", Checksum(*nmea, pos));
    return pos + static_cast<usize>(next);
}
}  // namespace vfrb::data::processor

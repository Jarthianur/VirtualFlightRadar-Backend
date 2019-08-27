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

#include "util/math.hpp"
#include "util/utility.hpp"

using namespace object;

namespace data::processor
{
AircraftProcessor::AircraftProcessor() : AircraftProcessor(std::numeric_limits<s32>::max()) {}

AircraftProcessor::AircraftProcessor(s32 maxDist) : Processor<object::Aircraft>(), m_maxDistance(maxDist) {}

void AircraftProcessor::process(Aircraft& aircraft) const
{
    calculateRelPosition(aircraft);
    if (m_distance <= m_maxDistance)
    {
        appendPFLAA(aircraft, appendPFLAU(aircraft, 0));
    }
    else
    {
        (*aircraft).clear();
    }
}

void AircraftProcessor::referTo(Location const& location, f64 atmPress)
{
    m_refLocation    = location;
    m_refAtmPressure = atmPress;
}

void AircraftProcessor::calculateRelPosition(Aircraft const& aircraft) const
{
    m_refRadLatitude       = math::radian(m_refLocation.latitude);
    m_refRadLongitude      = math::radian(m_refLocation.longitude);
    m_aircraftRadLongitude = math::radian(aircraft.location().longitude);
    m_aircraftRadLatitude  = math::radian(aircraft.location().latitude);
    m_lonDistance          = m_aircraftRadLongitude - m_refRadLongitude;
    m_latDistance          = m_aircraftRadLatitude - m_refRadLatitude;

    f64 a = std::pow(std::sin(m_latDistance / 2.0), 2.0) + std::cos(m_refRadLatitude) *
                                                               std::cos(m_aircraftRadLatitude) *
                                                               std::pow(std::sin(m_lonDistance / 2.0), 2.0);
    m_distance = math::doubleToInt(6371000.0 * (2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a))));

    m_relBearing = math::degree(
        std::atan2(std::sin(m_aircraftRadLongitude - m_refRadLongitude) * std::cos(m_aircraftRadLatitude),
                   std::cos(m_refRadLatitude) * std::sin(m_aircraftRadLatitude) -
                       std::sin(m_refRadLatitude) * std::cos(m_aircraftRadLatitude) *
                           std::cos(m_aircraftRadLongitude - m_refRadLongitude)));
    m_absBearing = std::fmod((m_relBearing + 360.0), 360.0);

    m_relNorth    = math::doubleToInt(std::cos(math::radian(m_absBearing)) * m_distance);
    m_relEast     = math::doubleToInt(std::sin(math::radian(m_absBearing)) * m_distance);
    m_relVertical = aircraft.targetType() == Aircraft::TargetType::TRANSPONDER ?
                        aircraft.location().altitude - math::icaoHeight(m_refAtmPressure) :
                        aircraft.location().altitude - m_refLocation.altitude;
}

usize AircraftProcessor::appendPFLAU(Aircraft& aircraft, usize pos) const
{
    int next = (*aircraft).format(pos, "$PFLAU,,,,1,0,%d,0,%d,%d,%s*", math::doubleToInt(m_relBearing),
                                  m_relVertical, m_distance, (*aircraft.id()).data());
    next += (*aircraft).format(pos, "%02x\r\n", math::checksum(**aircraft, pos));
    return pos + static_cast<usize>(next);
}

usize AircraftProcessor::appendPFLAA(Aircraft& aircraft, usize pos) const
{
    int next = 0;
    if (aircraft.hasFullInfo())
    {
        next = (*aircraft).format(
            pos, "$PFLAA,0,%d,%d,%d,%hhu,%s,%.3d,,%d,%3.1lf,%.1hhX*", m_relNorth, m_relEast, m_relVertical,
            util::raw_type(aircraft.idType()), (*aircraft.id()).data(),
            math::doubleToInt(math::saturate(aircraft.movement().heading, Aircraft::Movement::MIN_HEADING,
                                             Aircraft::Movement::MAX_HEADING)),
            math::doubleToInt(math::saturate(aircraft.movement().gndSpeed * math::MS_2_KMH,
                                             Aircraft::Movement::MIN_GND_SPEED,
                                             Aircraft::Movement::MAX_GND_SPEED)),
            math::saturate(aircraft.movement().climbRate, Aircraft::Movement::MIN_CLIMB_RATE,
                           Aircraft::Movement::MAX_CLIMB_RATE),
            util::raw_type(aircraft.aircraftType()));
    }
    else
    {
        next =
            (*aircraft).format(pos, "$PFLAA,0,%d,%d,%d,1,%s,,,,,%1hhX*", m_relNorth, m_relEast, m_relVertical,
                               (*aircraft.id()).data(), util::raw_type(aircraft.aircraftType()));
    }
    next += (*aircraft).format(pos, "%02x\r\n", math::checksum(**aircraft, pos));
    return pos + static_cast<usize>(next);
}
}  // namespace data::processor

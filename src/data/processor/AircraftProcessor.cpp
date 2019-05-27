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

namespace data
{
namespace processor
{
AircraftProcessor::AircraftProcessor() : AircraftProcessor(std::numeric_limits<std::int32_t>::max())
{}

AircraftProcessor::AircraftProcessor(std::int32_t maxDist)
    : Processor<object::Aircraft>(), m_maxDistance(maxDist)
{}

void AircraftProcessor::process(Aircraft& aircraft) const
{
    calculateRelPosition(aircraft);
    if (m_distance <= m_maxDistance)
    {
        appendPFLAA(aircraft, appendPFLAU(aircraft, 0));
    }
    else
    {
        aircraft.get_nmea().clear();
    }
}

void AircraftProcessor::referTo(const Position& position, double atmPress)
{
    m_refPosition    = position;
    m_refAtmPressure = atmPress;
}

void AircraftProcessor::calculateRelPosition(const Aircraft& aircraft) const
{
    m_refRadLatitude       = math::radian(m_refPosition.latitude);
    m_refRadLongitude      = math::radian(m_refPosition.longitude);
    m_aircraftRadLongitude = math::radian(aircraft.get_position().longitude);
    m_aircraftRadLatitude  = math::radian(aircraft.get_position().latitude);
    m_lonDistance          = m_aircraftRadLongitude - m_refRadLongitude;
    m_latDistance          = m_aircraftRadLatitude - m_refRadLatitude;

    double a = std::pow(std::sin(m_latDistance / 2.0), 2.0) +
               std::cos(m_refRadLatitude) * std::cos(m_aircraftRadLatitude) *
                   std::pow(std::sin(m_lonDistance / 2.0), 2.0);
    m_distance =
        math::doubleToInt(6371000.0 * (2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a))));

    m_relBearing = math::degree(std::atan2(
        std::sin(m_aircraftRadLongitude - m_refRadLongitude) * std::cos(m_aircraftRadLatitude),
        std::cos(m_refRadLatitude) * std::sin(m_aircraftRadLatitude) -
            std::sin(m_refRadLatitude) * std::cos(m_aircraftRadLatitude) *
                std::cos(m_aircraftRadLongitude - m_refRadLongitude)));
    m_absBearing = std::fmod((m_relBearing + 360.0), 360.0);

    m_relNorth    = math::doubleToInt(std::cos(math::radian(m_absBearing)) * m_distance);
    m_relEast     = math::doubleToInt(std::sin(math::radian(m_absBearing)) * m_distance);
    m_relVertical = aircraft.get_targetType() == Aircraft::TargetType::TRANSPONDER ?
                        aircraft.get_position().altitude - math::icaoHeight(m_refAtmPressure) :
                        aircraft.get_position().altitude - m_refPosition.altitude;
}

std::size_t AircraftProcessor::appendPFLAU(Aircraft& aircraft, std::size_t pos) const
{
    int bytes = std::snprintf(*aircraft.get_nmea() + pos, Aircraft::NMEA_SIZE - pos,
                              "$PFLAU,,,,1,0,%d,0,%d,%d,%s*", math::doubleToInt(m_relBearing),
                              m_relVertical, m_distance, *aircraft.get_id());
    bytes += finishSentence(*aircraft.get_nmea() + pos, Aircraft::NMEA_SIZE - pos,
                            Aircraft::NMEA_SIZE - pos - static_cast<std::size_t>(bytes));
    return pos + static_cast<std::size_t>(bytes);
}

std::size_t AircraftProcessor::appendPFLAA(Aircraft& aircraft, std::size_t pos) const
{
    int bytes = 0;
    if (aircraft.get_fullInfo())
    {
        bytes = std::snprintf(
            *aircraft.get_nmea() + pos, Aircraft::NMEA_SIZE - pos,
            "$PFLAA,0,%d,%d,%d,%hhu,%s,%03d,,%d,%3.1lf,%1hhX*", m_relNorth, m_relEast,
            m_relVertical, util::raw_type(aircraft.get_idType()), *aircraft.get_id(),
            math::doubleToInt(aircraft.get_movement().heading),
            math::doubleToInt(aircraft.get_movement().gndSpeed * math::MS_2_KMH),
            aircraft.get_movement().climbRate, util::raw_type(aircraft.get_aircraftType()));
    }
    else
    {
        bytes =
            std::snprintf(*aircraft.get_nmea() + pos, Aircraft::NMEA_SIZE - pos,
                          "$PFLAA,0,%d,%d,%d,1,%s,,,,,%1hhX*", m_relNorth, m_relEast, m_relVertical,
                          *aircraft.get_id(), util::raw_type(aircraft.get_aircraftType()));
    }
    bytes += finishSentence(*aircraft.get_nmea() + pos, Aircraft::NMEA_SIZE - pos,
                            Aircraft::NMEA_SIZE - pos - static_cast<std::size_t>(bytes));
    return pos + static_cast<std::size_t>(bytes);
}

}  // namespace processor
}  // namespace data

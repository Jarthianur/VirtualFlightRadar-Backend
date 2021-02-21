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

#include "data/processor/CAircraftProcessor.hpp"

#include "math/Math.hpp"
#include "object/CAircraft.hpp"
#include "object/CGpsPosition.hpp"
#include "util/Utility.hpp"

using vfrb::object::CAircraft;
using vfrb::object::SLocation;
using vfrb::str_util::Checksum;
using vfrb::str_util::StringInserter;

namespace vfrb::data::processor
{
CAircraftProcessor::CAircraftProcessor() : CAircraftProcessor(limits::s32::max()) {}

CAircraftProcessor::CAircraftProcessor(s32 maxDist_) : m_maxDistance(maxDist_) {}

void
CAircraftProcessor::Process(CAircraft const& aircraft_, StringInserter& nmea_) const {
    calculateRelPosition(aircraft_);
    if (m_distance <= m_maxDistance) {
        appendPflau(aircraft_, nmea_);
        appendPflaa(aircraft_, nmea_);
    }
}

void
CAircraftProcessor::ReferTo(SLocation const& loc_, f64 press_) {
    m_refLocation    = loc_;
    m_refAtmPressure = press_;
}

void
CAircraftProcessor::calculateRelPosition(CAircraft const& aircraft_) const {
    m_refRadLatitude       = math::Radian(m_refLocation.Latitude);
    m_refRadLongitude      = math::Radian(m_refLocation.Longitude);
    m_aircraftRadLongitude = math::Radian(aircraft_.Location().Longitude);
    m_aircraftRadLatitude  = math::Radian(aircraft_.Location().Latitude);
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
    m_relVertical = aircraft_.TargetType() == CAircraft::ETargetType::TRANSPONDER ?
                        aircraft_.Location().Altitude - math::IcaoHeight(m_refAtmPressure) :
                        aircraft_.Location().Altitude - m_refLocation.Altitude;
}

void
CAircraftProcessor::appendPflau(CAircraft const& aircraft_, StringInserter& nmea_) const {
    auto begin = nmea_.Format("$PFLAU,,,,1,0,{:d},0,{:d},{:d},{:s}*", math::DoubleToInt(m_relBearing),
                              m_relVertical, m_distance, *aircraft_.Id());
    nmea_.Format("{:02X}\r\n", Checksum(begin, nmea_.End()));
}

void
CAircraftProcessor::appendPflaa(CAircraft const& aircraft_, StringInserter& nmea_) const {
    String::const_iterator begin;
    if (aircraft_.HasFullInfo()) {
        begin = nmea_.Format(
            "$PFLAA,0,{:d},{:d},{:d},{:d},{:s},{:03d},,{:d},{:3.1f},{:1X}*", m_relNorth, m_relEast,
            m_relVertical, util::AsUnderlyingType(aircraft_.IdType()), *aircraft_.Id(),
            math::DoubleToInt(math::Saturate(aircraft_.Movement().Heading, CAircraft::SMovement::MIN_HEADING,
                                             CAircraft::SMovement::MAX_HEADING)),
            math::DoubleToInt(math::Saturate(aircraft_.Movement().GndSpeed * math::MS_2_KMH,
                                             CAircraft::SMovement::MIN_GND_SPEED,
                                             CAircraft::SMovement::MAX_GND_SPEED)),
            math::Saturate(aircraft_.Movement().ClimbRate, CAircraft::SMovement::MIN_CLIMB_RATE,
                           CAircraft::SMovement::MAX_CLIMB_RATE),
            util::AsUnderlyingType(aircraft_.AircraftType()));
    } else {
        begin =
            nmea_.Format("$PFLAA,0,{:d},{:d},{:d},1,{:s},,,,,{:1X}*", m_relNorth, m_relEast, m_relVertical,
                         *aircraft_.Id(), util::AsUnderlyingType(aircraft_.AircraftType()));
    }
    nmea_.Format("{:02X}\r\n", Checksum(begin, nmea_.End()));
}
}  // namespace vfrb::data::processor

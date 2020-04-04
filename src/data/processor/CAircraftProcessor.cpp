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

#include "data/processor/CAircraftProcessor.hpp"

#include <cmath>
#include <cstdio>
#include <limits>

#include "math/math.hpp"
#include "util/string_utils.hpp"
#include "util/utility.hpp"

using namespace vfrb::object;
using namespace vfrb::str_util;

namespace vfrb::data::processor
{
CAircraftProcessor::CAircraftProcessor() : CAircraftProcessor(std::numeric_limits<s32>::max()) {}

CAircraftProcessor::CAircraftProcessor(s32 maxDist_) : m_maxDistance(maxDist_) {}

void CAircraftProcessor::Process(CAircraft const& aircraft_, CStaticString<NMEA_SIZE>& nmea_) const {
    calculateRelPosition(aircraft_);
    if (m_distance <= m_maxDistance) {
        appendPflaa(aircraft_, nmea_, appendPflau(aircraft_, nmea_, 0));
    } else {
        nmea_.Clear();
    }
}

void CAircraftProcessor::ReferTo(SLocation const& loc_, f64 press_) {
    m_refLocation    = loc_;
    m_refAtmPressure = press_;
}

void CAircraftProcessor::calculateRelPosition(CAircraft const& aircraft_) const {
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

usize CAircraftProcessor::appendPflau(CAircraft const& aircraft_, CStaticString<NMEA_SIZE>& nmea_,
                                      usize idx_) const {
    int next = nmea_.Format(idx_, "$PFLAU,,,,1,0,%d,0,%d,%d,%s*", math::DoubleToInt(m_relBearing),
                            m_relVertical, m_distance, (*aircraft_.Id()).data());
    next += nmea_.Format(idx_, "%02x\r\n", Checksum(*nmea_, idx_));
    return idx_ + static_cast<usize>(next);
}

usize CAircraftProcessor::appendPflaa(CAircraft const& aircraft_, CStaticString<NMEA_SIZE>& nmea_,
                                      usize idx_) const {
    int next = 0;
    if (aircraft_.HasFullInfo()) {
        next = nmea_.Format(
            idx_, "$PFLAA,0,%d,%d,%d,%hhu,%s,%.3d,,%d,%3.1lf,%.1hhX*", m_relNorth, m_relEast, m_relVertical,
            util::AsUnderlyingType(aircraft_.IdType()), (*aircraft_.Id()).data(),
            math::DoubleToInt(math::Saturate(aircraft_.Movement().Heading, CAircraft::SMovement::MIN_HEADING,
                                             CAircraft::SMovement::MAX_HEADING)),
            math::DoubleToInt(math::Saturate(aircraft_.Movement().GndSpeed * math::MS_2_KMH,
                                             CAircraft::SMovement::MIN_GND_SPEED,
                                             CAircraft::SMovement::MAX_GND_SPEED)),
            math::Saturate(aircraft_.Movement().ClimbRate, CAircraft::SMovement::MIN_CLIMB_RATE,
                           CAircraft::SMovement::MAX_CLIMB_RATE),
            util::AsUnderlyingType(aircraft_.AircraftType()));
    } else {
        next = nmea_.Format(idx_, "$PFLAA,0,%d,%d,%d,1,%s,,,,,%1hhX*", m_relNorth, m_relEast, m_relVertical,
                            (*aircraft_.Id()).data(), util::AsUnderlyingType(aircraft_.AircraftType()));
    }
    next += nmea_.Format(idx_, "%02x\r\n", Checksum(*nmea_, idx_));
    return idx_ + static_cast<usize>(next);
}
}  // namespace vfrb::data::processor

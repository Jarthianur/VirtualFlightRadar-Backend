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

#include "data/processor/CGpsProcessor.hpp"

#include "math/Math.hpp"
#include "object/CGpsPosition.hpp"

using vfrb::object::CGpsPosition;
using vfrb::str_util::Checksum;
using vfrb::str_util::StringInserter;

namespace vfrb::data::processor
{
void
CGpsProcessor::Process(object::CGpsPosition const& pos_, StringInserter& nmea_) const {
    std::time_t now = std::time(nullptr);
    std::tm*    utc = std::gmtime(&now);
    evalPosition(pos_.Location().Latitude, pos_.Location().Longitude);
    appendGprmc(utc, nmea_);
    appendGpgga(pos_, utc, nmea_);
}

void
CGpsProcessor::appendGpgga(CGpsPosition const& pos_, std::tm const* utc_, StringInserter& nmea_) const {
    // XCSoar requires a fix quality of 1
    auto begin = nmea_.Format(
        "$GPGGA,{:%H%M%S},{:02.0f}{:07.4f},{:c},{:03.0f}{:07.4f},{:c},1,{:2d},1,{:d},M,{:.1f},M,,*", *utc_,
        m_degLatitude, m_minLatitude, m_directionSN, m_degLongitude, m_minLongitude, m_directionEW,
        pos_.NrOfSatellites(), pos_.Location().Altitude,
        math::Saturate(pos_.Geoid(), CGpsPosition::MIN_GEOID, CGpsPosition::MAX_GEOID));
    nmea_.Format("{:02X}\r\n", Checksum(begin, nmea_.End()));
}

void
CGpsProcessor::appendGprmc(std::tm const* utc_, StringInserter& nmea_) const {
    auto begin = nmea_.Format(
        "$GPRMC,{0:%H%M%S},A,{1:02.0f}{2:06.3f},{3:c},{4:03.0f}{5:06.3f},{6:c},0,0,{0:%d%m%y},001.0,W*",
        *utc_, m_degLatitude, m_minLatitude, m_directionSN, m_degLongitude, m_minLongitude, m_directionEW);
    nmea_.Format("{:02X}\r\n", Checksum(begin, nmea_.End()));
}

void
CGpsProcessor::evalPosition(f64 lat_, f64 lon_) const {
    m_directionSN  = (lat_ < 0) ? 'S' : 'N';
    m_directionEW  = (lon_ < 0) ? 'W' : 'E';
    m_degLatitude  = std::abs(std::floor(lat_));
    m_minLatitude  = std::abs(60.0 * (lat_ - m_degLatitude));
    m_degLongitude = std::abs(std::floor(lon_));
    m_minLongitude = std::abs(60.0 * (lon_ - m_degLongitude));
}
}  // namespace vfrb::data::processor

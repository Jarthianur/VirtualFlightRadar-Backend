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

#include <cmath>
#include <cstdio>

#include "math/Math.hpp"
#include "object/CGpsPosition.hpp"
#include "util/StringUtils.hpp"

using vfrb::object::CGpsPosition;
using vfrb::str_util::Checksum;

namespace vfrb::data::processor
{
void
CGpsProcessor::Process(object::CGpsPosition const& pos_, CStaticString<NMEA_SIZE>* nmea_) const {
    std::time_t now = std::time(nullptr);
    std::tm*    utc = std::gmtime(&now);
    evalPosition(pos_.Location().Latitude, pos_.Location().Longitude);
    appendGpgga(pos_, nmea_, utc, appendGprmc(nmea_, utc, 0));
}

auto
CGpsProcessor::appendGpgga(CGpsPosition const& pos_, CStaticString<NMEA_SIZE>* nmea_, std::tm const* utc_,
                           usize idx_) const -> usize {
    // XCSoar requires a fix quality of 1
    usize next = nmea_->Format(
        idx_, "$GPGGA,{:%H%M%S},{:02.0f}{:07.4f},{:c},{:03.0f}{:07.4f},{:c},1,{:2d},1,{:d},M,{:.1f},M,,*",
        *utc_, m_degLatitude, m_minLatitude, m_directionSN, m_degLongitude, m_minLongitude, m_directionEW,
        pos_.NrOfSatellites(), pos_.Location().Altitude,
        math::Saturate(pos_.Geoid(), CGpsPosition::MIN_GEOID, CGpsPosition::MAX_GEOID));
    next += nmea_->Format(idx_ + next, "{:02X}\r\n", Checksum(**nmea_, idx_));
    return idx_ + next;
}

auto
CGpsProcessor::appendGprmc(CStaticString<NMEA_SIZE>* nmea_, std::tm const* utc_, usize idx_) const -> usize {
    usize next = nmea_->Format(
        idx_, "$GPRMC,{0:%H%M%S},A,{1:02.0f}{2:06.3f},{3:c},{4:03.0f}{5:06.3f},{6:c},0,0,{0:%d%m%y},001.0,W*",
        *utc_, m_degLatitude, m_minLatitude, m_directionSN, m_degLongitude, m_minLongitude, m_directionEW);
    next += nmea_->Format(idx_ + next, "{:02X}\r\n", Checksum(**nmea_, idx_));
    return idx_ + next;
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

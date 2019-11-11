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

#include "data/processor/GpsProcessor.h"

#include <cmath>
#include <cstdio>

#include "math/math.hpp"
#include "util/string_utils.hpp"

using namespace vfrb::object;
using namespace vfrb::str_util;

namespace vfrb::data::processor
{
void CGpsProcessor::Process(object::CGpsPosition const& pos_, CString<NMEA_SIZE>& nmea_) const
{
    std::time_t now = std::time(nullptr);
    std::tm*    utc = std::gmtime(&now);
    evalPosition(pos_.Location().Latitude, pos_.Location().Longitude);
    appendGpgga(pos_, nmea_, utc, appendGprmc(nmea_, utc, 0));
}

usize CGpsProcessor::appendGpgga(CGpsPosition const& pos_, CString<NMEA_SIZE>& nmea_, std::tm const* utc_,
                                 usize idx_) const
{
    // As we use XCSoar as frontend, we need to set the fix quality to 1. It doesn't
    // support others.
    // "$GPGGA,%02d%02d%02d,%02.0lf%07.4lf,%c,%03.0lf%07.4lf,%c,%1d,%02d,1,%d,M,%.1lf,M,,*"
    int next = nmea_.Format(
        idx_, "$GPGGA,%.2d%.2d%.2d,%02.0lf%07.4lf,%c,%03.0lf%07.4lf,%c,1,%.2hhu,1,%d,M,%.1lf,M,,*",
        utc_->tm_hour, utc_->tm_min, utc_->tm_sec, m_degLatitude, m_minLatitude, m_directionSN,
        m_degLongitude, m_minLongitude, m_directionEW, /*pos.fixQa,*/ pos_.NrOfSatellites(),
        pos_.Location().Altitude,
        math::Saturate(pos_.Geoid(), CGpsPosition::MIN_GEOID, CGpsPosition::MAX_GEOID));
    next += nmea_.Format(idx_, "%02x\r\n", Checksum(*nmea_, idx_));
    return idx_ + static_cast<usize>(next);
}

usize CGpsProcessor::appendGprmc(CString<NMEA_SIZE>& nmea_, std::tm const* utc_, usize idx_) const
{
    int next = nmea_.Format(
        idx_, "$GPRMC,%.2d%.2d%.2d,A,%02.0lf%06.3lf,%c,%03.0lf%06.3lf,%c,0,0,%.2d%.2d%.2d,001.0,W*",
        utc_->tm_hour, utc_->tm_min, utc_->tm_sec, m_degLatitude, m_minLatitude, m_directionSN,
        m_degLongitude, m_minLongitude, m_directionEW, utc_->tm_mday, utc_->tm_mon + 1, utc_->tm_year - 100);
    next += nmea_.Format(idx_, "%02x\r\n", Checksum(*nmea_, idx_));
    return idx_ + static_cast<usize>(next);
}

void CGpsProcessor::evalPosition(f64 lat_, f64 lon_) const
{
    m_directionSN  = (lat_ < 0) ? 'S' : 'N';
    m_directionEW  = (lon_ < 0) ? 'W' : 'E';
    m_degLatitude  = std::abs(std::floor(lat_));
    m_minLatitude  = std::abs(60.0 * (lat_ - m_degLatitude));
    m_degLongitude = std::abs(std::floor(lon_));
    m_minLongitude = std::abs(60.0 * (lon_ - m_degLongitude));
}
}  // namespace vfrb::data::processor

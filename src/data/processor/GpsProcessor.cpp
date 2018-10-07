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

#include "GpsProcessor.h"

#include <cmath>
#include <cstdio>

using namespace object;

namespace data
{
namespace processor
{
GpsProcessor::GpsProcessor() : Processor<object::GpsPosition>()
{}

GpsProcessor::~GpsProcessor() noexcept
{}

void GpsProcessor::process(object::GpsPosition& position)
{
    std::time_t now = std::time(nullptr);
    std::tm* utc    = std::gmtime(&now);
    evalPosition(position.get_position().latitude, position.get_position().longitude);
    position.set_serialized(get_GPGGA(position, utc) + get_GPRMC(utc));
}

std::string GpsProcessor::get_GPGGA(const GpsPosition& position, const std::tm* utc)
{
    // As we use XCSoar as frontend, we need to set the fix quality to 1. It doesn't
    // support others.
    std::snprintf(
        m_buffer, sizeof(m_buffer),
        /*"$GPGGA,%02d%02d%02d,%02.0lf%07.4lf,%c,%03.0lf%07.4lf,%c,%1d,%02d,1,%d,M,%.1lf,M,,*"*/
        "$GPGGA,%02d%02d%02d,%02.0lf%07.4lf,%c,%03.0lf%07.4lf,%c,1,%02d,1,%d,M,%.1lf,M,,*",
        utc->tm_hour, utc->tm_min, utc->tm_sec, m_degLatitude, m_minLatitude, m_directionSN,
        m_degLongitude, m_minLongitude, m_directionEW, /*pos.fixQa,*/ position.get_nrOfSatellites(),
        position.get_position().altitude, position.get_geoid());
    std::string nmea_str(m_buffer);
    finishSentence(nmea_str);

    return nmea_str;
}

std::string GpsProcessor::get_GPRMC(const std::tm* utc)
{
    std::snprintf(
        m_buffer, sizeof(m_buffer),
        "$GPRMC,%02d%02d%02d,A,%02.0lf%05.2lf,%c,%03.0lf%05.2lf,%c,0,0,%02d%02d%02d,001.0,W*",
        utc->tm_hour, utc->tm_min, utc->tm_sec, m_degLatitude, m_minLatitude, m_directionSN,
        m_degLongitude, m_minLongitude, m_directionEW, utc->tm_mday, utc->tm_mon + 1,
        utc->tm_year - 100);
    std::string nmea_str(m_buffer);
    finishSentence(nmea_str);

    return nmea_str;
}

void GpsProcessor::evalPosition(double latitude, double longitude)
{
    m_directionSN  = (latitude < 0) ? 'S' : 'N';
    m_directionEW  = (longitude < 0) ? 'W' : 'E';
    m_degLatitude  = std::abs(std::floor(latitude));
    m_minLatitude  = std::abs(60.0 * (latitude - m_degLatitude));
    m_degLongitude = std::abs(std::floor(longitude));
    m_minLongitude = std::abs(60.0 * (longitude - m_degLongitude));
}

}  // namespace processor
}  // namespace data

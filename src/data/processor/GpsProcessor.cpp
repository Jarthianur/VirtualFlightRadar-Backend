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
#include <ctime>

using namespace object;

namespace data
{
namespace processor
{
GpsProcessor::GpsProcessor() : Processor<object::GpsPosition>()
{}

GpsProcessor::~GpsProcessor() noexcept
{}

void GpsProcessor::process(object::GpsPosition& rPosition)
{
    rPosition.set_serialized(genGpggaStr(rPosition) + genGprmcStr(rPosition));
}

std::string GpsProcessor::genGpggaStr(const GpsPosition& crPosition)
{
    std::time_t now = std::time(nullptr);
    std::tm* utc    = std::gmtime(&now);
    evalPosition(crPosition.get_position().latitude, crPosition.get_position().longitude);

    // As we use XCSoar as frontend, we need to set the fix quality to 1. It doesn't
    // support others.
    std::snprintf(
        mBuffer, sizeof(mBuffer),
        /*"$GPGGA,%02d%02d%02d,%02.0lf%07.4lf,%c,%03.0lf%07.4lf,%c,%1d,%02d,1,%d,M,%.1lf,M,,*"*/
        "$GPGGA,%02d%02d%02d,%02.0lf%07.4lf,%c,%03.0lf%07.4lf,%c,1,%02d,1,%d,M,%.1lf,M,,*",
        utc->tm_hour, utc->tm_min, utc->tm_sec, mtLatDeg, mtLatMin, mtLatStr, mtLonDeg, mtLonMin,
        mtLonStr, /*pos.fixQa,*/ crPosition.get_nrOfSatellites(),
        crPosition.get_position().altitude, crPosition.get_geoid());
    std::string nmea_str(mBuffer);
    finishSentence(nmea_str);

    return nmea_str;
}

std::string GpsProcessor::genGprmcStr(const GpsPosition& crPosition)
{
    std::time_t now = std::time(nullptr);
    std::tm* utc    = std::gmtime(&now);
    evalPosition(crPosition.get_position().latitude, crPosition.get_position().longitude);

    std::snprintf(
        mBuffer, sizeof(mBuffer),
        "$GPRMC,%02d%02d%02d,A,%02.0lf%05.2lf,%c,%03.0lf%05.2lf,%c,0,0,%02d%02d%02d,001.0,W*",
        utc->tm_hour, utc->tm_min, utc->tm_sec, mtLatDeg, mtLatMin, mtLatStr, mtLonDeg, mtLonMin,
        mtLonStr, utc->tm_mday, utc->tm_mon + 1, utc->tm_year - 100);
    std::string nmea_str(mBuffer);
    finishSentence(nmea_str);

    return nmea_str;
}

void GpsProcessor::evalPosition(double vLatitude, double vLongitude)
{
    mtLatStr = (vLatitude < 0) ? 'S' : 'N';
    mtLonStr = (vLongitude < 0) ? 'W' : 'E';
    mtLatDeg = std::abs(std::floor(vLatitude));
    mtLatMin = std::abs(60.0 * (vLatitude - mtLatDeg));
    mtLonDeg = std::abs(std::floor(vLongitude));
    mtLonMin = std::abs(60.0 * (vLongitude - mtLonDeg));
}

}  // namespace processor
}  // namespace data

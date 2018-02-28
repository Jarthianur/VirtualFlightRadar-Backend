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

#include "AircraftProcessor.h"

#include <cmath>
#include <cstdio>

#include "../../util/Math.hpp"

using namespace data::object;

namespace data
{
namespace processing
{
AircraftProcessor::AircraftProcessor()
    : mMaxDistance(std::numeric_limits<std::int32_t>::max())
{}

AircraftProcessor::AircraftProcessor(std::int32_t vMaxDist) : mMaxDistance(vMaxDist)
{}

AircraftProcessor::~AircraftProcessor() noexcept
{}

void AircraftProcessor::process(Aircraft& crAircraft, const GpsPosition& crRelPos,
                                double vAtmPress)
{
    calcRelativePosition(crAircraft, crRelPos, vAtmPress);

    if(mtDist > mMaxDistance)
    {
        crAircraft.setSerialized("");
        return;
    }

    std::string nmea_str;
    buildPflauStr(crAircraft, nmea_str);
    buildPflaaStr(crAircraft, nmea_str);

    crAircraft.setSerialized(nmea_str);
}

void AircraftProcessor::calcRelativePosition(const Aircraft& crAircraft,
                                             const GpsPosition& crRelPos,
                                             double vAtmPress)
{
    mtRadLatB   = util::math::radian(crRelPos.latitude);
    mtRadLongB  = util::math::radian(crRelPos.longitude);
    mtRadLongAc = util::math::radian(crAircraft.getLongitude());
    mtRadLatAc  = util::math::radian(crAircraft.getLatitude());
    mtLongDist  = mtRadLongAc - mtRadLongB;
    mtLatDist   = mtRadLatAc - mtRadLatB;

    double a = std::pow(std::sin(mtLatDist / 2.0), 2.0)
               + std::cos(mtRadLatB) * std::cos(mtRadLatAc)
                     * std::pow(std::sin(mtLongDist / 2.0), 2.0);
    mtDist = util::math::dToI(6371000.0
                              * (2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a))));

    mtBearingRel = util::math::degree(
        std::atan2(std::sin(mtRadLongAc - mtRadLongB) * std::cos(mtRadLatAc),
                   std::cos(mtRadLatB) * std::sin(mtRadLatAc)
                       - std::sin(mtRadLatB) * std::cos(mtRadLatAc)
                             * std::cos(mtRadLongAc - mtRadLongB)));
    mtBearingAbs = std::fmod((mtBearingRel + 360.0), 360.0);

    mtRelN = util::math::dToI(std::cos(util::math::radian(mtBearingAbs)) * mtDist);
    mtRelE = util::math::dToI(std::sin(util::math::radian(mtBearingAbs)) * mtDist);
    mtRelV = crAircraft.getTargetType() == Aircraft::TargetType::TRANSPONDER
                 ? crAircraft.getAltitude() - util::math::calcIcaoHeight(vAtmPress)
                 : crAircraft.getAltitude() - crRelPos.altitude;
}

void AircraftProcessor::buildPflauStr(const Aircraft& crAircraft, std::string& rDestStr)
{
    std::snprintf(mBuffer, AP_BUFF_S, "$PFLAU,,,,1,0,%d,0,%d,%d,%s*",
                  util::math::dToI(mtBearingRel), mtRelV, mtDist,
                  crAircraft.getId().c_str());
    finishSentence(rDestStr);
}

void AircraftProcessor::buildPflaaStr(const Aircraft& crAircraft, std::string& rDestStr)
{
    if(crAircraft.hasFullInfo())
    {
        std::snprintf(
            mBuffer, AP_BUFF_S, "$PFLAA,0,%d,%d,%d,%d,%s,%03d,,%d,%3.1lf,%1x*", mtRelN,
            mtRelE, mtRelV, static_cast<std::int32_t>(crAircraft.getIdType()),
            crAircraft.getId().c_str(), util::math::dToI(crAircraft.getHeading()),
            util::math::dToI(crAircraft.getGndSpeed() * util::math::MS_2_KMH),
            crAircraft.getClimbRate(),
            static_cast<std::uint32_t>(crAircraft.getAircraftType()));
    }
    else
    {
        std::snprintf(mBuffer, AP_BUFF_S, "$PFLAA,0,%d,%d,%d,1,%s,,,,,%1x*", mtRelN,
                      mtRelE, mtRelV, crAircraft.getId().c_str(),
                      static_cast<std::uint32_t>(crAircraft.getAircraftType()));
    }
    finishSentence(rDestStr);
}

void AircraftProcessor::finishSentence(std::string& rDestStr)
{
    std::int32_t csum = util::math::checksum(mBuffer, AP_BUFF_S);
    rDestStr.append(mBuffer);
    std::snprintf(mBuffer, AP_L_BUFF_S, "%02x\r\n", csum);
    rDestStr.append(mBuffer);
}

}  // namespace aircraft
}

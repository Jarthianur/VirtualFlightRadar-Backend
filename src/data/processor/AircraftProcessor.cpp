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
#include <limits>

#include "../../Math.hpp"

using namespace data::object;

namespace data
{
namespace processor
{
AircraftProcessor::AircraftProcessor()
    : AircraftProcessor(std::numeric_limits<std::int32_t>::max())
{}

AircraftProcessor::AircraftProcessor(std::int32_t vMaxDist)
    : Processor<object::Aircraft>(), mMaxDistance(vMaxDist)
{}

AircraftProcessor::~AircraftProcessor() noexcept
{}

std::string AircraftProcessor::process(const Aircraft& crAircraft)
{
    calcRelativePosition(crAircraft);
    return mtDist <= mMaxDistance ? genPflauStr(crAircraft) + genPflaaStr(crAircraft)
                                  : "";
}

void AircraftProcessor::setRelatives(const GpsPosition& crRelPos, double vAtmPress)
{
    mtReferedPosition = crRelPos;
    mtAtmPressure = vAtmPress;
}

void AircraftProcessor::calcRelativePosition(const Aircraft& crAircraft)
{
    mtRadLatB   = math::radian(mtReferedPosition.latitude);
    mtRadLongB  = math::radian(mtReferedPosition.longitude);
    mtRadLongAc = math::radian(crAircraft.getLongitude());
    mtRadLatAc  = math::radian(crAircraft.getLatitude());
    mtLongDist  = mtRadLongAc - mtRadLongB;
    mtLatDist   = mtRadLatAc - mtRadLatB;

    double a = std::pow(std::sin(mtLatDist / 2.0), 2.0)
               + std::cos(mtRadLatB) * std::cos(mtRadLatAc)
                     * std::pow(std::sin(mtLongDist / 2.0), 2.0);
    mtDist = math::doubleToInt(6371000.0
                               * (2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a))));

    mtBearingRel = math::degree(
        std::atan2(std::sin(mtRadLongAc - mtRadLongB) * std::cos(mtRadLatAc),
                   std::cos(mtRadLatB) * std::sin(mtRadLatAc)
                       - std::sin(mtRadLatB) * std::cos(mtRadLatAc)
                             * std::cos(mtRadLongAc - mtRadLongB)));
    mtBearingAbs = std::fmod((mtBearingRel + 360.0), 360.0);

    mtRelN = math::doubleToInt(std::cos(math::radian(mtBearingAbs)) * mtDist);
    mtRelE = math::doubleToInt(std::sin(math::radian(mtBearingAbs)) * mtDist);
    mtRelV = crAircraft.getTargetType() == Aircraft::TargetType::TRANSPONDER
                 ? crAircraft.getAltitude() - math::icaoHeight(mtAtmPressure)
                 : crAircraft.getAltitude() - mtReferedPosition.altitude;
}

std::string AircraftProcessor::genPflauStr(const Aircraft& crAircraft)
{
    std::snprintf(mBuffer, sizeof(mBuffer), "$PFLAU,,,,1,0,%d,0,%d,%d,%s*",
                  math::doubleToInt(mtBearingRel), mtRelV, mtDist,
                  crAircraft.getId().c_str());
    std::string tmp(mBuffer);
    finishSentence(tmp);
    return tmp;
}

std::string AircraftProcessor::genPflaaStr(const Aircraft& crAircraft)
{
    if(crAircraft.hasFullInfo())
    {
        std::snprintf(
            mBuffer, sizeof(mBuffer), "$PFLAA,0,%d,%d,%d,%d,%s,%03d,,%d,%3.1lf,%1x*",
            mtRelN, mtRelE, mtRelV, static_cast<std::int32_t>(crAircraft.getIdType()),
            crAircraft.getId().c_str(), math::doubleToInt(crAircraft.getHeading()),
            math::doubleToInt(crAircraft.getGndSpeed() * math::MS_2_KMH),
            crAircraft.getClimbRate(),
            static_cast<std::uint32_t>(crAircraft.getAircraftType()));
    }
    else
    {
        std::snprintf(mBuffer, sizeof(mBuffer), "$PFLAA,0,%d,%d,%d,1,%s,,,,,%1x*", mtRelN,
                      mtRelE, mtRelV, crAircraft.getId().c_str(),
                      static_cast<std::uint32_t>(crAircraft.getAircraftType()));
    }
    std::string tmp(mBuffer);
    finishSentence(tmp);
    return tmp;
}

}  // namespace aircraft
}

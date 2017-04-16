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

#include "../config/Configuration.h"
#include "../data/ClimateData.h"
#include "../util/Math.hpp"
#include "../vfrb/VFRB.h"
#include "Aircraft.h"

AircraftProcessor::AircraftProcessor()
        : mBaseLat(0.0),
          mBaseLong(0.0),
          mBaseAlt(0)
{
}

AircraftProcessor::AircraftProcessor(double b_lat, double b_long, std::int32_t b_alt)
        : mBaseLat(b_lat),
          mBaseLong(b_long),
          mBaseAlt(b_alt)
{
    mRadLongB = Math::radian(mBaseLong);
    mRadLatB = Math::radian(mBaseLat);
}

AircraftProcessor::~AircraftProcessor() noexcept
{
}

void AircraftProcessor::init(double lat, double lon, std::int32_t alt)
{
    mBaseLat = lat;
    mBaseLong = lon;
    mBaseAlt = alt;
    mRadLongB = Math::radian(mBaseLong);
    mRadLatB = Math::radian(mBaseLat);
}

std::string AircraftProcessor::process(Aircraft& ac)
{
    calcRelPosToBase(ac);

    if (mtDist > Configuration::filter_maxDist)
    {
        return "";
    }

    std::string nmea_str;

    //PFLAU
    std::snprintf(mBuffer, AP_BUFF_S, "$PFLAU,,,,1,0,%d,0,%d,%d,%s*",
                  Math::dToI(mtBearingRel), mtRelV, mtDist, ac.getID().c_str());
    std::int32_t csum = Math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, AP_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    //PFLAA
    if (ac.isFullInfo())
    {
        std::snprintf(mBuffer, AP_BUFF_S, "$PFLAA,0,%d,%d,%d,%u,%s,%03d,,%d,%3.1lf,%1x*",
                      mtRelN, mtRelE, mtRelV, ac.getIDtype(), ac.getID().c_str(),
                      Math::dToI(ac.getHeading()),
                      Math::dToI(ac.getGndSpeed() * Math::ms2kmh), ac.getClimbR(),
                      ac.getAircraftT());
    }
    else
    {
        std::snprintf(mBuffer, AP_BUFF_S, "$PFLAA,0,%d,%d,%d,1,%s,,,,,%1x*", mtRelN,
                      mtRelE, mtRelV, ac.getID().c_str(), ac.getAircraftT());
    }
    csum = Math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, AP_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    return nmea_str;
}

void AircraftProcessor::calcRelPosToBase(Aircraft& ac)
{
    mtRadLongAc = Math::radian(ac.getLongitude());
    mtRadLatAc = Math::radian(ac.getLatitude());
    mtLongDist = mtRadLongAc - mRadLongB;
    mtLatDist = mtRadLatAc - mRadLatB;

    mtAval = std::pow(std::sin(mtLatDist / 2.0), 2.0)
            + std::cos(mRadLatB) * std::cos(mtRadLatAc)
              * std::pow(std::sin(mtLongDist / 2.0), 2.0);
    mtDist = Math::dToI(
            6371000.0 * (2.0 * std::atan2(std::sqrt(mtAval), std::sqrt(1.0 - mtAval))));

    mtBearingRel = Math::degree(
            std::atan2(
                    std::sin(mtRadLongAc - mRadLongB) * std::cos(mtRadLatAc),
                    std::cos(mRadLatB) * std::sin(mtRadLatAc) - std::sin(mRadLatB)
                            * std::cos(mtRadLatAc) * std::cos(mtRadLongAc - mRadLongB)));
    mtBearingAbs = std::fmod((mtBearingRel + 360.0), 360.0);

    mtRelN = Math::dToI(std::cos(Math::radian(mtBearingAbs)) * mtDist);
    mtRelE = Math::dToI(std::sin(Math::radian(mtBearingAbs)) * mtDist);
    mtRelV =
            ac.isAltQNE() ?
                    ac.getAltitude() - Math::calcIcaoHeight(
                            VFRB::msClimateData.getPress()) : ac.getAltitude() - mBaseAlt;
}

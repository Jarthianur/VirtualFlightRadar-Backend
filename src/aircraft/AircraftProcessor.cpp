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
#include "../data/GPSData.h"
#include "../data/SensorData.h"
#include "../util/Math.hpp"
#include "../vfrb/VFRB.h"
#include "Aircraft.h"

AircraftProcessor::AircraftProcessor()
{
}

AircraftProcessor::~AircraftProcessor() noexcept
{
}

std::string AircraftProcessor::process(const Aircraft& r_ac)
{
    calcRelPosToBase(r_ac);

    if (mtDist > Configuration::filter_maxDist)
    {
        return "";
    }

    std::string nmea_str;

    //PFLAU
    std::snprintf(mBuffer, AP_BUFF_S, "$PFLAU,,,,1,0,%d,0,%d,%d,%s*",
            Math::dToI(mtBearingRel), mtRelV, mtDist, r_ac.getID().c_str());
    std::int32_t csum = Math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, AP_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    //PFLAA
    if (r_ac.isFullInfo())
    {
        std::snprintf(mBuffer, AP_BUFF_S,
                "$PFLAA,0,%d,%d,%d,%u,%s,%03d,,%d,%3.1lf,%1x*", mtRelN, mtRelE,
                mtRelV, r_ac.getIDtype(), r_ac.getID().c_str(),
                Math::dToI(r_ac.getHeading()),
                Math::dToI(r_ac.getGndSpeed() * Math::ms2kmh), r_ac.getClimbR(),
                r_ac.getAircraftT());
    } else
    {
        std::snprintf(mBuffer, AP_BUFF_S, "$PFLAA,0,%d,%d,%d,1,%s,,,,,%1x*",
                mtRelN, mtRelE, mtRelV, r_ac.getID().c_str(),
                r_ac.getAircraftT());
    }
    csum = Math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, AP_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    return nmea_str;
}

void AircraftProcessor::calcRelPosToBase(const Aircraft& r_ac)
{
    mtRadLatB = Math::radian(VFRB::msGPSdata.getBaseLat());
    mtRadLongB = Math::radian(VFRB::msGPSdata.getBaseLong());
    mtRadLongAc = Math::radian(r_ac.getLongitude());
    mtRadLatAc = Math::radian(r_ac.getLatitude());
    mtLongDist = mtRadLongAc - mtRadLongB;
    mtLatDist = mtRadLatAc - mtRadLatB;
    double a = std::pow(std::sin(mtLatDist / 2.0), 2.0)
            + std::cos(mtRadLatB) * std::cos(mtRadLatAc)
              * std::pow(std::sin(mtLongDist / 2.0), 2.0);
    mtDist = Math::dToI(
            6371000.0 * (2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a))));
    mtBearingRel =
            Math::degree(
                    std::atan2(
                            std::sin(mtRadLongAc - mtRadLongB) * std::cos(
                                    mtRadLatAc),
                            std::cos(mtRadLatB) * std::sin(mtRadLatAc) - std::sin(
                                                                                 mtRadLatB)
                                                                         * std::cos(
                                                                                 mtRadLatAc)
                                                                         * std::cos(
                                                                                 mtRadLongAc - mtRadLongB)));
    mtBearingAbs = std::fmod((mtBearingRel + 360.0), 360.0);
    mtRelN = Math::dToI(std::cos(Math::radian(mtBearingAbs)) * mtDist);
    mtRelE = Math::dToI(std::sin(Math::radian(mtBearingAbs)) * mtDist);
    mtRelV =
            r_ac.getTargetT() == Aircraft::TargetType::TRANSPONDER ?
                    r_ac.getAltitude() - Math::calcIcaoHeight(
                            VFRB::msSensorData.getPress()) :
                    r_ac.getAltitude() - VFRB::msGPSdata.getBaseAlt();
}

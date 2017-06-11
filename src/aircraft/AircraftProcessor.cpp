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

namespace aircraft
{

AircraftProcessor::AircraftProcessor()
{
}

AircraftProcessor::~AircraftProcessor() noexcept
{
}

std::string AircraftProcessor::process(const Aircraft& cr_ac)
{
    calcRelPosToBase(cr_ac);

    if (mtDist > config::Configuration::filter_maxDist)
    {
        return "";
    }

    std::string nmea_str;

    //PFLAU
    std::snprintf(mBuffer, AP_BUFF_S, "$PFLAU,,,,1,0,%d,0,%d,%d,%s*",
            util::math::dToI(mtBearingRel), mtRelV, mtDist,
            cr_ac.getID().c_str());
    std::int32_t csum = util::math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, AP_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    //PFLAA
    if (cr_ac.isFullInfo())
    {
        std::snprintf(mBuffer, AP_BUFF_S,
                "$PFLAA,0,%d,%d,%d,%u,%s,%03d,,%d,%3.1lf,%1x*", mtRelN, mtRelE,
                mtRelV, cr_ac.getIDtype(), cr_ac.getID().c_str(),
                util::math::dToI(cr_ac.getHeading()),
                util::math::dToI(cr_ac.getGndSpeed() * util::math::MS_2_KMH),
                cr_ac.getClimbR(), cr_ac.getAircraftT());
    } else
    {
        std::snprintf(mBuffer, AP_BUFF_S, "$PFLAA,0,%d,%d,%d,1,%s,,,,,%1x*",
                mtRelN, mtRelE, mtRelV, cr_ac.getID().c_str(),
                cr_ac.getAircraftT());
    }
    csum = util::math::checksum(mBuffer, sizeof(mBuffer));
    nmea_str.append(mBuffer);
    std::snprintf(mBuffer, AP_L_BUFF_S, "%02x\r\n", csum);
    nmea_str.append(mBuffer);

    return nmea_str;
}

void AircraftProcessor::calcRelPosToBase(const Aircraft& cr_ac)
{
    mtRadLatB = util::math::radian(vfrb::VFRB::msGPSdata.getBaseLat());
    mtRadLongB = util::math::radian(vfrb::VFRB::msGPSdata.getBaseLong());
    mtRadLongAc = util::math::radian(cr_ac.getLongitude());
    mtRadLatAc = util::math::radian(cr_ac.getLatitude());
    mtLongDist = mtRadLongAc - mtRadLongB;
    mtLatDist = mtRadLatAc - mtRadLatB;
    double a = std::pow(std::sin(mtLatDist / 2.0), 2.0)
            + std::cos(mtRadLatB) * std::cos(mtRadLatAc)
              * std::pow(std::sin(mtLongDist / 2.0), 2.0);
    mtDist = util::math::dToI(
            6371000.0 * (2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a))));
    mtBearingRel =
            util::math::degree(
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
    mtRelN = util::math::dToI(
            std::cos(util::math::radian(mtBearingAbs)) * mtDist);
    mtRelE = util::math::dToI(
            std::sin(util::math::radian(mtBearingAbs)) * mtDist);
    mtRelV =
            cr_ac.getTargetT() == Aircraft::TargetType::TRANSPONDER ?
                    cr_ac.getAltitude() - util::math::calcIcaoHeight(
                            vfrb::VFRB::msSensorData.getPress()) :
                    cr_ac.getAltitude() - vfrb::VFRB::msGPSdata.getBaseAlt();
}

}  // namespace aircraft

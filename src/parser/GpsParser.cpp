/*
 Copyright_License {

 Copyright (C) 2017 VirtualFlightRadar-Backend
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

#include "GpsParser.h"

#include <boost/regex.hpp>
#include <stdexcept>

#include "../data/GpsData.h"
#include "../util/Math.hpp"
#include "../VFRB.h"

namespace parser
{

#define GPS_ASSUME_GOOD       (1)
#define GPS_NR_SATS_GOOD      (7)
#define GPS_FIX_GOOD          (1)
#define GPS_HOR_DILUTION_GOOD (1.0)

GpsParser::GpsParser()
        : Parser(),
          mGpggaRe(
                  "^\\$[A-Z]{2}GGA,\\d{6},(\\d{4}\\.\\d{3,4}),([NS]),(\\d{5}\\.\\d{3,4}),([EW]),(\\d),(\\d{2}),(\\d+(?:\\.\\d+)?),(\\d+(?:\\.\\d+)?),M,(\\d+(?:\\.\\d+)?),M,,\\*[0-9A-F]{2}\\s+?$",
                  boost::regex::optimize | boost::regex::icase)
{
}

GpsParser::~GpsParser() noexcept
{
}

std::int32_t GpsParser::unpack(const std::string& cr_msg, std::int32_t prio)
noexcept
{
    try
    {
        std::int32_t csum = std::stoi(cr_msg.substr(cr_msg.rfind('*') + 1, 2), nullptr,
                16);
        if (csum != util::math::checksum(cr_msg.c_str(), cr_msg.length()))
        {
            return MSG_UNPACK_IGN;
        }
    } catch (const std::logic_error& e)
    {
        return MSG_UNPACK_ERR;
    }

    boost::smatch match;
    if (boost::regex_match(cr_msg, match, mGpggaRe))
    {
        double dilution = 0.0;
        try
        {
            //latitude
            mtGpsPos.position.latitude = util::math::dmToDeg(std::stod(match.str(1)));
            if (match.str(2).compare("S") == 0)
            {
                mtGpsPos.position.latitude = -mtGpsPos.position.latitude;
            }

            //longitude
            mtGpsPos.position.longitude = util::math::dmToDeg(std::stod(match.str(3)));
            if (match.str(4).compare("W") == 0)
            {
                mtGpsPos.position.longitude = -mtGpsPos.position.longitude;
            }

            //fix
            mtGpsPos.fixQa = std::stoi(match.str(5));
            //sats
            mtGpsPos.nrSats = std::stoi(match.str(6));
            //dilution
            dilution = std::stod(match.str(7));
            //altitude
            mtGpsPos.position.altitude = util::math::dToI(std::stod(match.str(8)));
            //geoid
            mtGpsPos.geoid = std::stod(match.str(9));
        } catch (const std::logic_error& e)
        {
            return MSG_UNPACK_ERR;
        }
        VFRB::msGpsData.setBasePos(prio, mtGpsPos);

        if (mtGpsPos.nrSats >= GPS_NR_SATS_GOOD && mtGpsPos.fixQa >= GPS_FIX_GOOD
                && dilution <= GPS_HOR_DILUTION_GOOD)
        {
            return GPS_ASSUME_GOOD;
        }
    } else
    {
        return MSG_UNPACK_IGN;
    }

    return MSG_UNPACK_SUC;
}

}  // namespace parser

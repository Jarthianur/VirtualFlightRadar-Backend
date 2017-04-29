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

#include "GPSParser.h"

#include <boost/regex.hpp>
#include <stdexcept>

#include "../data/GPSData.h"
#include "../util/Math.hpp"
#include "../util/Priority.h"
#include "../vfrb/VFRB.h"

#define GPS_ASSUME_GOOD  (1)
#define GPS_NR_SATS_GOOD (7)
#define GPS_FIX_GOOD     (1)

GPSParser::GPSParser()
        : Parser(),
          mGpggaRE(
                  "^\\$[A-Z]{2}GGA,\\d{6},(\\d{4}\\.\\d{3,4}),([NS]),(\\d{5}\\.\\d{3,4}),([EW]),(\\d),(\\d{2}),[0-9.]+?,(\\d+\\.?\\d+?),M,(\\d+\\.?\\d+?),M,,\\*[0-9A-F]{2}\\s+?$",
                  boost::regex::optimize | boost::regex::icase)
{
}

GPSParser::~GPSParser() noexcept
{
}

std::int32_t GPSParser::unpack(const std::string& msg, Priority prio) noexcept
{
    try
    {
        std::int32_t csum = std::stoi(msg.substr(msg.rfind('*') + 1, 2), nullptr, 16);
        if (csum != Math::checksum(msg.c_str(), msg.length()))
        {
            return MSG_UNPACK_IGN;
        }
    }
    catch (const std::logic_error& e)
    {
        return MSG_UNPACK_ERR;
    }

    boost::smatch match;
    if (boost::regex_match(msg, match, mGpggaRE))
    {
        try
        {
            //latitude
            mtGPSpos.position.latitude = Math::dmToDeg(std::stod(match.str(1)));
            if (match.str(2).compare("S") == 0)
            {
                mtGPSpos.position.latitude = -mtGPSpos.position.latitude;
            }

            //longitude
            mtGPSpos.position.longitude = Math::dmToDeg(std::stod(match.str(3)));
            if (match.str(4).compare("W") == 0)
            {
                mtGPSpos.position.longitude = -mtGPSpos.position.longitude;
            }

            //fix
            mtGPSpos.fixQa = std::stoi(match.str(5));
            //sats
            mtGPSpos.nrSats = std::stoi(match.str(6));
            //altitude
            mtGPSpos.position.altitude = Math::dToI(std::stod(match.str(7)));
            //geoid
            mtGPSpos.geoid = std::stod(match.str(8));
        }
        catch (const std::logic_error& e)
        {
            return MSG_UNPACK_ERR;
        }

        VFRB::msGPSdata.setGGAstr(prio, msg);
        VFRB::msGPSdata.setBasePos(prio, mtGPSpos);

        if (mtGPSpos.nrSats >= GPS_NR_SATS_GOOD && mtGPSpos.fixQa == GPS_FIX_GOOD)
        {
            return GPS_ASSUME_GOOD;
        }
    }
    else if (msg.find("RMC") != std::string::npos)
    {
        VFRB::msGPSdata.setRMCstr(prio, msg);
    }
    else
    {
        return MSG_UNPACK_IGN;
    }

    return MSG_UNPACK_SUC;
}

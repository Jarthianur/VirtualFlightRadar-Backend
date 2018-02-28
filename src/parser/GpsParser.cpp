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

#include <stdexcept>

#include "../util/Math.hpp"

/// Define regex match groups for GGA
#define RE_GGA_LAT 1
#define RE_GGA_LAT_DIR 2
#define RE_GGA_LONG 3
#define RE_GGA_LONG_DIR 4
#define RE_GGA_FIX 5
#define RE_GGA_SAT 6
#define RE_GGA_DIL 7
#define RE_GGA_ALT 8
#define RE_GGA_GEOID 9

namespace parser
{
const boost::regex GpsParser::msGpggaRe(
    "^\\$[A-Z]{2}GGA,\\d{6},(\\d{4}\\.\\d{3,4}),([NS]),(\\d{5}\\.\\d{3,4}),([EW]),(\\d),(\\d{2}),(\\d+(?:\\.\\d+)?),(\\d+(?:\\.\\d+)?),M,(\\d+(?:\\.\\d+)?),M,,\\*[0-9A-F]{2}\\s*?$",
    boost::regex::optimize | boost::regex::icase);

GpsParser::GpsParser() : Parser()
{}

GpsParser::~GpsParser() noexcept
{}

bool GpsParser::unpack(const std::string& cr_msg,
                       data::object::ExtGpsPosition& r_pos) noexcept
{
    try
    {
        std::int32_t csum
            = std::stoi(cr_msg.substr(cr_msg.rfind('*') + 1, 2), nullptr, 16);
        if(csum != util::math::checksum(cr_msg.c_str(), cr_msg.length()))
        {
            return false;
        }

        boost::smatch match;
        if(boost::regex_match(cr_msg, match, msGpggaRe))
        {
            // latitude
            r_pos.position.latitude
                = util::math::dmToDeg(std::stod(match.str(RE_GGA_LAT)));
            if(match.str(RE_GGA_LAT_DIR).compare("S") == 0)
            {
                r_pos.position.latitude = -r_pos.position.latitude;
            }

            // longitude
            r_pos.position.longitude
                = util::math::dmToDeg(std::stod(match.str(RE_GGA_LONG)));
            if(match.str(RE_GGA_LONG_DIR).compare("W") == 0)
            {
                r_pos.position.longitude = -r_pos.position.longitude;
            }

            // fix
            r_pos.fixQa = std::stoi(match.str(RE_GGA_FIX));
            // sats
            r_pos.nrSats = std::stoi(match.str(RE_GGA_SAT));
            // dilution
            r_pos.dilution = std::stod(match.str(RE_GGA_DIL));
            // altitude
            r_pos.position.altitude = util::math::dToI(std::stod(match.str(RE_GGA_ALT)));
            // geoid
            r_pos.geoid = std::stod(match.str(RE_GGA_GEOID));
        }
        else
        {
            return false;
        }
    }
    catch(const std::logic_error&)
    {
        return false;
    }
    return true;
}

}  // namespace parser

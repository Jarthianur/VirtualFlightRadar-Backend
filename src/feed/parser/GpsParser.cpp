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

#include "../../Math.hpp"

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

namespace feed
{
namespace parser
{
const boost::regex GpsParser::msGpggaRe(
    "^\\$[A-Z]{2}GGA,\\d{6},(\\d{4}\\.\\d{3,4}),([NS]),(\\d{5}\\.\\d{3,4}),([EW]),(\\d),(\\d{2}),(\\d+(?:\\.\\d+)?),(\\d+(?:\\.\\d+)?),M,(\\d+(?:\\.\\d+)?),M,,\\*[0-9A-F]{2}\\s*?$",
    boost::regex::optimize | boost::regex::icase);

GpsParser::GpsParser() : Parser()
{}

GpsParser::~GpsParser() noexcept
{}

bool GpsParser::unpack(const std::string& crStr,
                       data::object::ExtGpsPosition& rPosition) noexcept
{
    try
    {
        boost::smatch match;
        return std::stoi(crStr.substr(crStr.rfind('*') + 1, 2), nullptr, 16)
                   == math::checksum(crStr.c_str(), crStr.length())
               && boost::regex_match(crStr, match, msGpggaRe)
               && parsePosition(match, rPosition);
    }
    catch(const std::logic_error&)
    {
        return false;
    }
}

bool GpsParser::parsePosition(const boost::smatch& crMatch,
                              data::object::ExtGpsPosition& rPosition)
{
    rPosition.position.latitude = math::dmToDeg(std::stod(crMatch.str(RE_GGA_LAT)));
    if(crMatch.str(RE_GGA_LAT_DIR).compare("S") == 0)
    {
        rPosition.position.latitude = -rPosition.position.latitude;
    }
    rPosition.position.longitude = math::dmToDeg(std::stod(crMatch.str(RE_GGA_LONG)));
    if(crMatch.str(RE_GGA_LONG_DIR).compare("W") == 0)
    {
        rPosition.position.longitude = -rPosition.position.longitude;
    }
    rPosition.fixQa             = std::stoi(crMatch.str(RE_GGA_FIX));
    rPosition.nrSats            = std::stoi(crMatch.str(RE_GGA_SAT));
    rPosition.dilution          = std::stod(crMatch.str(RE_GGA_DIL));
    rPosition.position.altitude = math::doubleToInt(std::stod(crMatch.str(RE_GGA_ALT)));
    rPosition.geoid             = std::stod(crMatch.str(RE_GGA_GEOID));
    return true;
}
}
}  // namespace parser

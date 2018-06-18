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

/// @def RE_GGA_TIME
/// GGA regex match capture group of time
#define RE_GGA_TIME 1

/// @def RE_GGA_LAT
/// GGA regex match capture group of latitude
#define RE_GGA_LAT 2

/// @def RE_GGA_LAT_DIR
/// GGA regex match capture group of latitude orientation
#define RE_GGA_LAT_DIR 3

/// @def RE_GGA_LON
/// GGA regex match capture group of longitude
#define RE_GGA_LON 4

/// @def RE_GGA_LON_DIR
/// GGA regex match capture group of longitude orientation
#define RE_GGA_LON_DIR 5

/// @def RE_GGA_FIX
/// GGA regex match capture group of fix quality
#define RE_GGA_FIX 6

/// @def RE_GGA_SAT
/// GGA regex match capture group of sitallite count
#define RE_GGA_SAT 7

/// @def RE_GGA_DIL
/// GGA regex match capture group of dilution
#define RE_GGA_DIL 8

/// @def RE_GGA_ALT
/// GGA regex match capture group of altitude
#define RE_GGA_ALT 9

/// @def RE_GGA_GEOID
/// GGA regex match capture group of geoid separation
#define RE_GGA_GEOID 10

namespace feed
{
namespace parser
{
const boost::regex GpsParser::msGpggaRe(
    "^\\$[A-Z]{2}GGA,(\\d{6}),(\\d{4}\\.\\d{3,4}),([NS]),(\\d{5}\\.\\d{3,4}),([EW]),(\\d),(\\d{2}),(\\d+(?:\\.\\d+)?),(\\d+(?:\\.\\d+)?),M,(\\d+(?:\\.\\d+)?),M,,\\*[0-9A-F]{2}\\s*?$",
    boost::regex::optimize | boost::regex::icase);

GpsParser::GpsParser() : Parser()
{}

GpsParser::~GpsParser() noexcept
{}

bool GpsParser::unpack(const std::string& crStr, object::GpsPosition& rPosition) noexcept
{
    try
    {
        boost::smatch match;
        return std::stoi(crStr.substr(crStr.rfind('*') + 1, 2), nullptr, 16)
                   == math::checksum(crStr.c_str(), crStr.length())
               && boost::regex_match(crStr, match, msGpggaRe) && parsePosition(match, rPosition);
    }
    catch(const std::logic_error&)
    {
        return false;
    }
}

bool GpsParser::parsePosition(const boost::smatch& crMatch, object::GpsPosition& rPosition)
{
    object::Position pos;
    pos.latitude = math::dmToDeg(std::stod(crMatch.str(RE_GGA_LAT)));

    if(crMatch.str(RE_GGA_LAT_DIR).compare("S") == 0)
    {
        pos.latitude = -pos.latitude;
    }
    pos.longitude = math::dmToDeg(std::stod(crMatch.str(RE_GGA_LON)));

    if(crMatch.str(RE_GGA_LON_DIR).compare("W") == 0)
    {
        pos.longitude = -pos.longitude;
    }
    pos.altitude = math::doubleToInt(std::stod(crMatch.str(RE_GGA_ALT)));
    rPosition.setPosition(pos);
    rPosition.setTimeStamp(
        object::TimeStamp(crMatch.str(RE_GGA_TIME), object::TimeStamp::Format::HHMMSS));
    rPosition.setFixQuality(std::stoi(crMatch.str(RE_GGA_FIX)));
    rPosition.setNrOfSatellites(std::stoi(crMatch.str(RE_GGA_SAT)));
    rPosition.setDilution(std::stod(crMatch.str(RE_GGA_DIL)));
    rPosition.setGeoid(std::stod(crMatch.str(RE_GGA_GEOID)));
    return true;
}

}  // namespace parser
}  // namespace feed

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

#include "feed/parser/GpsParser.h"

#include <stdexcept>
#include <type_traits>

#include "object/GpsPosition.h"
#include "object/Timestamp.hpp"
#include "object/impl/DateTimeImplBoost.h"
#include "util/math.hpp"

using namespace object;

namespace feed
{
namespace parser
{
const boost::regex GpsParser::s_GPGGA_RE(
    "^\\$[A-Z]{2}GGA,(\\d{6}),(\\d{4}\\.\\d{3,4}),([NS]),(\\d{5}\\.\\d{3,4}),([EW]),(\\d),(\\d{2}),(\\d+(?:\\.\\d+)?),(\\d+(?:\\.\\d+)?),M,(\\d+(?:\\.\\d+)?),M,,\\*[0-9A-F]{2}\\s*?$",
    boost::regex::optimize | boost::regex::icase);

GpsParser::GpsParser() : Parser<GpsPosition>() {}

GpsPosition GpsParser::unpack(const std::string& sentence, std::uint32_t priority) const
{
    try
    {
        boost::smatch match;
        if (std::stoi(sentence.substr(sentence.rfind('*') + 1, 2), nullptr, 16) ==
                math::checksum(sentence.c_str(), sentence.length()) &&
            boost::regex_match(sentence, match, s_GPGGA_RE))
        {
            return parsePosition(match, priority);
        }
    }
    catch (const std::logic_error&)
    {}
    throw UnpackError();
}

GpsPosition GpsParser::parsePosition(const boost::smatch& match, std::uint32_t priority) const
{
    auto latitude = math::dmToDeg(std::stod(match.str(RE_GGA_LAT)));
    if (match.str(RE_GGA_LAT_DIR).compare("S") == 0)
    {
        latitude = -latitude;
    }
    auto longitude = math::dmToDeg(std::stod(match.str(RE_GGA_LON)));
    if (match.str(RE_GGA_LON_DIR).compare("W") == 0)
    {
        longitude = -longitude;
    }
    auto altitude = math::doubleToInt(std::stod(match.str(RE_GGA_ALT)));
    return GpsPosition{priority,
                       {latitude, longitude, altitude},
                       std::stod(match.str(RE_GGA_GEOID)),
                       std::stod(match.str(RE_GGA_DIL)),
                       static_cast<std::uint8_t>(std::stoi(match.str(RE_GGA_SAT))),
                       static_cast<std::int8_t>(std::stoi(match.str(RE_GGA_FIX))),
                       Timestamp<time::DateTimeImplBoost>(match.str(RE_GGA_TIME), time::Format::HHMMSS)};
}

}  // namespace parser
}  // namespace feed

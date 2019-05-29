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
#include "object/TimeStamp.hpp"
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

bool GpsParser::unpack(const std::string& sentence, GpsPosition& position) noexcept
{
    try
    {
        boost::smatch match;
        return std::stoi(sentence.substr(sentence.rfind('*') + 1, 2), nullptr, 16) ==
                   math::checksum(sentence.c_str(), sentence.length()) &&
               boost::regex_match(sentence, match, s_GPGGA_RE) && parsePosition(match, position);
    }
    catch (const std::logic_error&)
    {
        return false;
    }
}

bool GpsParser::parsePosition(const boost::smatch& match, GpsPosition& position)
{
    Location pos;
    pos.latitude = math::dmToDeg(std::stod(match.str(RE_GGA_LAT)));

    if (match.str(RE_GGA_LAT_DIR).compare("S") == 0)
    {
        pos.latitude = -pos.latitude;
    }
    pos.longitude = math::dmToDeg(std::stod(match.str(RE_GGA_LON)));

    if (match.str(RE_GGA_LON_DIR).compare("W") == 0)
    {
        pos.longitude = -pos.longitude;
    }
    pos.altitude          = math::doubleToInt(std::stod(match.str(RE_GGA_ALT)));
    position.m_location   = pos;
    position.m_timeStamp  = TimeStamp<time::DateTimeImplBoost>(match.str(RE_GGA_TIME), time::Format::HHMMSS);
    position.m_fixQuality = static_cast<decltype(position.m_fixQuality)>(std::stoi(match.str(RE_GGA_FIX)));
    position.m_nrOfSatellites =
        static_cast<decltype(position.m_nrOfSatellites)>(std::stoi(match.str(RE_GGA_SAT)));
    position.m_dilution = (std::stod(match.str(RE_GGA_DIL)));
    position.m_geoid    = (std::stod(match.str(RE_GGA_GEOID)));
    return true;
}

}  // namespace parser
}  // namespace feed

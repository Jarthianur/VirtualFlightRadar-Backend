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

#include "feed/parser/AprsParser.h"

#include <limits>
#include <stdexcept>

#include "object/Timestamp.hpp"
#include "object/impl/DateTimeImplBoost.h"
#include "util/math.hpp"

namespace feed
{
using namespace object;

namespace parser
{
const boost::regex AprsParser::s_APRS_RE(
    "^(?:\\S+?)>APRS,\\S+?(?:,\\S+?)?:/(\\d{6})h(\\d{4}\\.\\d{2})([NS])[\\S\\s]+?(\\d{5}\\.\\d{2})([EW])[\\S\\s]+?(?:(\\d{3})/(\\d{3}))?/A=(\\d{6})\\s+?([\\S\\s]+?)$",
    boost::regex::optimize | boost::regex::icase);

const boost::regex AprsParser::s_APRSExtRE(
    "^(?:[\\S\\s]+?)?id([0-9A-F]{2})([0-9A-F]{6})\\s?(?:([\\+-]\\d{3})fpm\\s+?)?(?:([\\+-]\\d+?\\.\\d+?)rot)?(?:[\\S\\s]+?)?$",
    boost::regex::optimize | boost::regex::icase);

std::int32_t AprsParser::s_maxHeight = std::numeric_limits<std::int32_t>::max();

AprsParser::AprsParser() : Parser<Aircraft>() {}

Aircraft AprsParser::unpack(const std::string& sentence, std::uint32_t priority) const
{
    boost::smatch match, com_match;
    if ((!sentence.empty() && sentence[0] == '#') || !boost::regex_match(sentence, match, s_APRS_RE) ||
        !boost::regex_match(match.str(RE_APRS_COM), com_match, s_APRSExtRE))
    {
        throw UnpackError();
    }
    auto meta = parseComment(com_match);
    // relies on TargetType::FLARM in ctor
    return {priority,
            std::get<0>(meta),
            std::get<1>(meta),
            std::get<2>(meta),
            parseLocation(match),
            parseMovement(match, com_match),
            parseTimeStamp(match)};
}

Location AprsParser::parseLocation(const boost::smatch& match) const
{
    Location pos;
    try
    {
        pos.latitude = math::dmToDeg(std::stod(match.str(RE_APRS_LAT)));
        if (match.str(RE_APRS_LAT_DIR).compare("S") == 0)
        {
            pos.latitude = -pos.latitude;
        }
        pos.longitude = math::dmToDeg(std::stod(match.str(RE_APRS_LON)));
        if (match.str(RE_APRS_LON_DIR).compare("W") == 0)
        {
            pos.longitude = -pos.longitude;
        }
        pos.altitude = math::doubleToInt(std::stod(match.str(RE_APRS_ALT)) * math::FEET_2_M);
    }
    catch (const std::logic_error&)
    {
        throw UnpackError();
    }
    if (pos.altitude <= s_maxHeight)
    {
        return pos;
    }
    throw UnpackError();
}

AprsParser::MetaInfo AprsParser::parseComment(const boost::smatch& match) const
{
    try
    {
        return {match.str(RE_APRS_COM_ID),
                static_cast<Aircraft::IdType>(std::stoi(match.str(RE_APRS_COM_TYPE), nullptr, 16) & 0x03),
                static_cast<Aircraft::AircraftType>(
                    (std::stoi(match.str(RE_APRS_COM_TYPE), nullptr, 16) & 0x7C) >> 2)};
    }
    catch (const std::logic_error&)
    {}
    throw UnpackError();
}

Aircraft::Movement AprsParser::parseMovement(const boost::smatch& match, const boost::smatch& comMatch) const
{
    // This needs to be split later to parse independently.
    try
    {
        return {std::stod(match.str(RE_APRS_HEAD)), std::stod(match.str(RE_APRS_GND_SPD)) * math::KTS_2_MS,
                std::stod(comMatch.str(RE_APRS_COM_CR)) * math::FPM_2_MS};
    }
    catch (const std::logic_error&)
    {}
    throw UnpackError();
}

Timestamp<time::DateTimeImplBoost> AprsParser::parseTimeStamp(const boost::smatch& match) const
{
    try
    {
        return Timestamp<time::DateTimeImplBoost>(match.str(RE_APRS_TIME), time::Format::HHMMSS);
    }
    catch (const std::invalid_argument&)
    {}
    throw UnpackError();
}

}  // namespace parser
}  // namespace feed

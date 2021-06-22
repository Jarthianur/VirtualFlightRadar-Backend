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

#include "feed/parser/FlarmParser.h"

#include <chrono>
#include <ctime>
#include <stdexcept>

#include "util/math.hpp"

#define RE_PFLAA_RN 1
#define RE_PFLAA_RE 2
#define RE_PFLAA_RV 3
#define RE_PFLAA_IDT 4
#define RE_PFLAA_ID 5
#define RE_PFLAA_HD 6
#define RE_PFLAA_GS 7
#define RE_PFLAA_CR 8
#define RE_PFLAA_AT 9

using namespace object;

namespace feed
{
namespace parser
{
std::int32_t FlarmParser::s_maxHeight = std::numeric_limits<std::int32_t>::max();

const boost::regex FlarmParser::s_FLARM_RE(
    "^\\$PFLAA,0,([-]?\\d+?),([-]?\\d+?),([-]?\\d+?),(\\d+?),(\\S{6}),(\\d{3}),,(\\d+?),([-]?\\d+?\\.\\d+?),([0-9A-F]{1,2})\\*(?:\\S{2})$",
    boost::regex::optimize | boost::regex::icase);

std::string getTime()
{
    std::time_t tt       = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char        time[32] = "";
    std::strftime(time, 32, "%H%M%S", gmtime(&tt));
    return std::string(time);
}

// This makes use of an approximation and is not accurate. For experimental use only.
bool FlarmParser::parseAircraft(const boost::smatch& match, Aircraft& aircraft)
{
    try
    {
        std::int32_t relNorth = std::stoi(match.str(RE_PFLAA_RN));
        std::int32_t relEast  = std::stoi(match.str(RE_PFLAA_RE));
        std::int32_t relVert  = std::stoi(match.str(RE_PFLAA_RV));
        Position     refPos   = aircraft.get_position();

        Position aPos;
        aPos.latitude  = refPos.latitude + (relNorth / 6371000.0) * (180.0 / math::PI);
        aPos.longitude = refPos.longitude + (relEast / 6371000.0) * (180.0 / math::PI) /
                                                std::cos(refPos.latitude * math::PI / 180.0);
        aPos.altitude = refPos.altitude + relVert;

        if (aPos.altitude > s_maxHeight)
        {
            return false;
        }

        aircraft.set_position(aPos);
        aircraft.set_aircraftType(
            static_cast<Aircraft::AircraftType>(std::stoul(match.str(RE_PFLAA_AT))));
        aircraft.set_idType(Aircraft::IdType::FLARM);
        aircraft.set_targetType(Aircraft::TargetType::FLARM);
        aircraft.set_fullInfo(true);
        aircraft.set_id(match.str(RE_PFLAA_ID));
        aircraft.set_movement({std::stod(match.str(RE_PFLAA_GS)), std::stod(match.str(RE_PFLAA_HD)),
                               std::stod(match.str(RE_PFLAA_CR))});
        aircraft.set_timeStamp(
            TimeStamp<timestamp::DateTimeImplBoost>(getTime(), timestamp::Format::HHMMSS));
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

FlarmParser::FlarmParser() : Parser<Aircraft>() {}

bool FlarmParser::unpack(const std::string& sentence, Aircraft& aircraft) noexcept
{
    try
    {
        boost::smatch match;
        return std::stoi(sentence.substr(sentence.rfind('*') + 1, 2), nullptr, 16) ==
                   math::checksum(sentence.c_str(), sentence.length()) &&
               boost::regex_match(sentence, match, s_FLARM_RE) && parseAircraft(match, aircraft);
    }
    catch (const std::logic_error&)
    {}
    return false;
}

}  // namespace parser
}  // namespace feed

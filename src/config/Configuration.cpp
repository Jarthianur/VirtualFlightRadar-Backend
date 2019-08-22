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

#include "config/Configuration.h"

#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "config/ConfigReader.h"
#include "util/Logger.hpp"

using namespace util;
using namespace std::literals;

namespace config
{
using Number    = std::variant<s32, u64, f64>;
using OptNumber = std::optional<Number>;

constexpr auto     LOG_PREFIX = "(Config) ";
static auto const& logger     = Logger::instance();

/**
 * @brief Convert a string to number.
 * @tparam T    The number type
 * @param str The string to convert
 * @return an optional number, which may be invalid
 */
template<typename T>
OptNumber stringToNumber(str const& str)
{
    std::stringstream ss(str);
    T                 result;
    if (ss >> result)
    {
        return Number(result);
    }
    return {};
}

/**
 * @brief Check an optional Number to be valid.
 * @param number The optinonal Number
 * @param path   The key path
 * @return the number value
 * @throw std::invalid_argument if the Number is invalid
 */
Number checkNumber(OptNumber const& number, char const* path)
{
    if (!number)
    {
        throw std::invalid_argument("invalid value at "s + path);
    }
    return *number;
}

/**
 * @brief Trim a string on both sides.
 * @param str The string to trim
 * @return the trimmed string
 */
str& trimString(str& str)
{
    usize f = str.find_first_not_of(' ');
    if (f != str::npos)
    {
        str = str.substr(f);
    }
    f = str.find_last_not_of(' ');
    if (f != str::npos)
    {
        str = str.substr(0, f + 1);
    }
    return str;
}

/**
 * @brief Split a string, separated at commata.
 * @param str The string to split
 * @return a list of strings
 */
inline std::list<str> split(str const& s, char delim = ',')
{
    std::list<str>    list;
    std::stringstream ss;
    ss.str(s);
    str item;

    while (std::getline(ss, item, delim))
    {
        list.push_back(trimString(item));
    }
    return list;
}

Configuration::Configuration(std::istream& stream)
try :
    m_properties(ConfigReader(stream).read()),
    groundMode(!m_properties.property(PATH_GND_MODE).empty()),
    gpsPosition(resolvePosition(m_properties)),
    atmPressure(std::get<f64>(
        checkNumber(stringToNumber<f64>(m_properties.property(PATH_PRESSURE, "1013.25")), PATH_PRESSURE))),
    maxHeight(resolveFilter(m_properties, PATH_MAX_HEIGHT)),
    maxDistance(resolveFilter(m_properties, PATH_MAX_DIST)),
    serverPort(resolveServerPort(m_properties)),
    feedNames(split(m_properties.property(PATH_FEEDS))),
    feedProperties(resolveFeeds(m_properties))
{
    dumpInfo();
}
catch (std::exception const& e)
{
    logger.error(LOG_PREFIX, "init: ", e.what());
}

object::GpsPosition Configuration::resolvePosition(Properties const& properties) const
{
    object::Location pos;
    pos.latitude = std::get<f64>(
        checkNumber(stringToNumber<f64>(properties.property(PATH_LATITUDE, "0.0")), PATH_LATITUDE));
    pos.longitude = std::get<f64>(
        checkNumber(stringToNumber<f64>(properties.property(PATH_LONGITUDE, "0.0")), PATH_LONGITUDE));
    pos.altitude = std::get<s32>(
        checkNumber(stringToNumber<s32>(properties.property(PATH_ALTITUDE, "0")), PATH_ALTITUDE));
    f64 geoid =
        std::get<f64>(checkNumber(stringToNumber<f64>(properties.property(PATH_GEOID, "0.0")), PATH_GEOID));
    return object::GpsPosition(0, pos, geoid);
}

std::uint16_t Configuration::resolveServerPort(const Properties& properties) const
{
    try
    {
        u64 port = std::get<u64>(checkNumber(
            stringToNumber<u64>(properties.property(PATH_SERVER_PORT, "4353")), PATH_SERVER_PORT));
        if (port > std::numeric_limits<u16>::max())
        {
            throw std::invalid_argument("invalid port number");
        }
        return port & 0xFFFF;
    }
    catch (std::logic_error const& e)
    {
        logger.warn(LOG_PREFIX, "resolving server port: ", e.what());
        return 4353;
    }
}

s32 Configuration::resolveFilter(Properties const& properties, char const* path) const
{
    try
    {
        s32 filter = std::get<s32>(checkNumber(stringToNumber<s32>(properties.property(path, "-1")), path));
        return filter < 0 ? std::numeric_limits<s32>::max() : filter;
    }
    catch (std::invalid_argument const&)
    {
        return std::numeric_limits<s32>::max();
    }
}

std::unordered_map<str, Properties> Configuration::resolveFeeds(Properties const& properties)
{
    std::unordered_map<str, Properties> map;
    for (auto const& it : feedNames)
    {
        try
        {
            map.emplace(it, properties.section(it));
        }
        catch (std::out_of_range const& e)
        {
            logger.warn(LOG_PREFIX, "resolving feeds: ", e.what(), " for ", it);
        }
    }
    return map;
}

void Configuration::dumpInfo() const
{
    logger.info(LOG_PREFIX, PATH_LATITUDE, ": ", gpsPosition.location().latitude);
    logger.info(LOG_PREFIX, PATH_LONGITUDE, ": ", gpsPosition.location().longitude);
    logger.info(LOG_PREFIX, PATH_ALTITUDE, ": ", gpsPosition.location().altitude);
    logger.info(LOG_PREFIX, PATH_GEOID, ": ", gpsPosition.geoid());
    logger.info(LOG_PREFIX, PATH_PRESSURE, ": ", atmPressure);
    logger.info(LOG_PREFIX, PATH_MAX_HEIGHT, ": ", maxHeight);
    logger.info(LOG_PREFIX, PATH_MAX_DIST, ": ", maxDistance);
    logger.info(LOG_PREFIX, PATH_SERVER_PORT, ": ", serverPort);
    logger.info(LOG_PREFIX, PATH_GND_MODE, ": ", groundMode ? "Yes" : "No");
    logger.info(LOG_PREFIX, "number of feeds: ", feedProperties.size());
}
}  // namespace config

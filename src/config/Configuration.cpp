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

#include <cstdint>
#include <limits>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include "config/ConfigReader.h"
#include "util/Logger.hpp"

using namespace util;

namespace config
{
using Number    = boost::variant<std::int32_t, std::uint64_t, double>;
using OptNumber = boost::optional<Number>;

/**
 * @brief Convert a string to number.
 * @tparam T    The number type
 * @param str The string to convert
 * @return an optional number, which may be invalid
 */
template<typename T>
OptNumber stringToNumber(const std::string& str)
{
    std::stringstream ss(str);
    T                 result;
    if (ss >> result)
    {
        return Number(result);
    }
    return boost::none;
}

/**
 * @brief Check an optional Number to be valid.
 * @param number The optinonal Number
 * @param path   The key path
 * @return the number value
 * @throw std::invalid_argument if the Number is invalid
 */
Number checkNumber(const OptNumber& number, const char* path)
{
    if (!number)
    {
        throw std::invalid_argument(std::string("invalid value at ") + path);
    }
    return *number;
}

/**
 * @brief Trim a string on both sides.
 * @param str The string to trim
 * @return the trimmed string
 */
std::string& trimString(std::string& str)
{
    std::size_t f = str.find_first_not_of(' ');
    if (f != std::string::npos)
    {
        str = str.substr(f);
    }
    std::size_t l = str.find_last_not_of(' ');
    if (l != std::string::npos)
    {
        str = str.substr(0, l + 1);
    }
    return str;
}

/**
 * @brief Split a string, separated at commata.
 * @param str The string to split
 * @return a list of strings
 */
inline std::list<std::string> split(const std::string& str, char delim = ',')
{
    std::list<std::string> list;
    std::stringstream      ss;
    ss.str(str);
    std::string item;

    while (std::getline(ss, item, delim))
    {
        list.push_back(trimString(item));
    }
    return list;
}

Configuration::Configuration(std::istream& stream)
try : m_properties(ConfigReader(stream).read()),
      groundMode(!m_properties.get_property(PATH_GND_MODE).empty()),
      gpsPosition(resolvePosition(m_properties)),
      atmPressure(boost::get<double>(checkNumber(
          stringToNumber<double>(m_properties.get_property(PATH_PRESSURE, "1013.25")), PATH_PRESSURE))),
      maxHeight(resolveFilter(m_properties, PATH_MAX_HEIGHT)),
      maxDistance(resolveFilter(m_properties, PATH_MAX_DIST)),
      serverPort(resolveServerPort(m_properties)),
      feedNames(split(m_properties.get_property(PATH_FEEDS))),
      feedProperties(resolveFeeds(m_properties))
{
    dumpInfo();
}
catch (const std::exception& e)
{
    logger.error(LOG_PREFIX, "init: ", e.what());
}

object::GpsPosition Configuration::resolvePosition(const Properties& properties) const
{
    object::Location pos;
    pos.latitude = boost::get<double>(
        checkNumber(stringToNumber<double>(properties.get_property(PATH_LATITUDE, "0.0")), PATH_LATITUDE));
    pos.longitude = boost::get<double>(
        checkNumber(stringToNumber<double>(properties.get_property(PATH_LONGITUDE, "0.0")), PATH_LONGITUDE));
    pos.altitude = boost::get<std::int32_t>(checkNumber(
        stringToNumber<std::int32_t>(properties.get_property(PATH_ALTITUDE, "0")), PATH_ALTITUDE));
    double geoid = boost::get<double>(
        checkNumber(stringToNumber<double>(properties.get_property(PATH_GEOID, "0.0")), PATH_GEOID));
    return object::GpsPosition(0, pos, geoid);
}

std::uint16_t Configuration::resolveServerPort(const Properties& properties) const
{
    try
    {
        std::uint64_t port = boost::get<std::uint64_t>(
            checkNumber(stringToNumber<std::uint64_t>(properties.get_property(PATH_SERVER_PORT, "4353")),
                        PATH_SERVER_PORT));
        if (port > std::numeric_limits<std::uint16_t>::max())
        {
            throw std::invalid_argument("invalid port number");
        }
        return port & 0xFFFF;
    }
    catch (const std::logic_error& e)
    {
        logger.warn(LOG_PREFIX, "resolving server port: ", e.what());
        return 4353;
    }
}

std::int32_t Configuration::resolveFilter(const Properties& properties, const char* path) const
{
    try
    {
        std::int32_t filter = boost::get<std::int32_t>(
            checkNumber(stringToNumber<std::int32_t>(properties.get_property(path, "-1")), path));
        return filter < 0 ? std::numeric_limits<std::int32_t>::max() : filter;
    }
    catch (const std::invalid_argument&)
    {
        return std::numeric_limits<std::int32_t>::max();
    }
}

std::unordered_map<std::string, Properties> Configuration::resolveFeeds(const Properties& properties)
{
    std::unordered_map<std::string, Properties> map;
    for (const auto& it : feedNames)
    {
        try
        {
            map.emplace(it, properties.get_propertySection(it));
        }
        catch (const std::out_of_range& e)
        {
            logger.warn(LOG_PREFIX, "resolving feeds: ", e.what(), " for ", it);
        }
    }
    return map;
}

void Configuration::dumpInfo() const
{
    logger.info(LOG_PREFIX, PATH_LATITUDE, ": ", gpsPosition.getLocation().latitude);
    logger.info(LOG_PREFIX, PATH_LONGITUDE, ": ", gpsPosition.getLocation().longitude);
    logger.info(LOG_PREFIX, PATH_ALTITUDE, ": ", gpsPosition.getLocation().altitude);
    logger.info(LOG_PREFIX, PATH_GEOID, ": ", gpsPosition.getGeoid());
    logger.info(LOG_PREFIX, PATH_PRESSURE, ": ", atmPressure);
    logger.info(LOG_PREFIX, PATH_MAX_HEIGHT, ": ", maxHeight);
    logger.info(LOG_PREFIX, PATH_MAX_DIST, ": ", maxDistance);
    logger.info(LOG_PREFIX, PATH_SERVER_PORT, ": ", serverPort);
    logger.info(LOG_PREFIX, PATH_GND_MODE, ": ", groundMode ? "Yes" : "No");
    logger.info(LOG_PREFIX, "number of feeds: ", feedProperties.size());
}

}  // namespace config

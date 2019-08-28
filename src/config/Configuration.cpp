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
#include <utility>
#include <variant>

#include "config/ConfigReader.h"
#include "error/Exception.hpp"
#include "util/Logger.hpp"
#include "util/utility.hpp"

using namespace vfrb::util;
using namespace std::literals;

namespace vfrb::config
{
using Number = std::variant<s32, u64, f64>;

namespace error
{
ConfigurationError::ConfigurationError() : vfrb::error::Exception() {}

char const* ConfigurationError::what() const noexcept
{
    return "configuration initialization failed";
}

class ConversionError : public vfrb::error::Exception
{
    str const m_msg;

public:
    ConversionError(str const& str, char const* path)
        : vfrb::error::Exception(), m_msg("invalid value at "s + path + " [" + str + "]")
    {}
    ~ConversionError() noexcept override = default;

    char const* what() const noexcept override
    {
        return m_msg.c_str();
    }
};
}  // namespace error

constexpr auto     LOG_PREFIX = "(Config) ";
static auto const& logger     = Logger::instance();

/**
 * @brief Convert a string to number.
 * @tparam T    The number type
 * @param str The string to convert
 * @return an optional number, which may be invalid
 */
template<typename T>
Number strToNumber(str const& str, char const* path)
{
    std::stringstream ss(str);
    T                 result;
    if (ss >> result)
    {
        return Number(result);
    }
    throw error::ConversionError(str, path);
}

/**
 * @brief Trim a string on both sides.
 * @param str The string to trim
 * @return the trimmed string
 */
inline str& trimString(str& str)
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
    groundMode(m_properties.property(PATH_GND_MODE, "no") == "no"),
    gpsPosition(resolvePosition(m_properties)),
    atmPressure(std::get<f64>(strToNumber<f64>(m_properties.property(PATH_PRESSURE, "1013.25"),
                                               PATH_PRESSURE))),
    maxHeight(resolveFilter(m_properties, PATH_MAX_HEIGHT)),
    maxDistance(resolveFilter(m_properties, PATH_MAX_DIST)),
    serverPort(resolveServerPort(m_properties)),
    feedNames(split(m_properties.property(PATH_FEEDS, ""))),
    feedProperties(resolveFeeds(m_properties))
{
    dumpInfo();
}
catch (vfrb::error::Exception const& e)
{
    logger.error(LOG_PREFIX, "init: ", e.what());
    throw error::ConfigurationError();
}

object::GpsPosition Configuration::resolvePosition(Properties const& properties) const
{
    object::Location pos;
    pos.latitude = std::get<f64>(strToNumber<f64>(properties.property(PATH_LATITUDE, "0.0"), PATH_LATITUDE));
    pos.longitude =
        std::get<f64>(strToNumber<f64>(properties.property(PATH_LONGITUDE, "0.0"), PATH_LONGITUDE));
    pos.altitude = std::get<s32>(strToNumber<s32>(properties.property(PATH_ALTITUDE, "0"), PATH_ALTITUDE));
    f64 geoid    = std::get<f64>(strToNumber<f64>(properties.property(PATH_GEOID, "0.0"), PATH_GEOID));
    return object::GpsPosition(0, pos, geoid);
}

u16 Configuration::resolveServerPort(const Properties& properties) const
{
    try
    {
        u64 port =
            std::get<u64>(strToNumber<u64>(properties.property(PATH_SERVER_PORT, "4353"), PATH_SERVER_PORT));
        util::checkLimits<u64>(port, 0, std::numeric_limits<u16>::max());
        return port & 0xFFFF;
    }
    catch (vfrb::error::Exception const& e)
    {
        logger.warn(LOG_PREFIX, "resolving server port: ", e.what());
        return 4353;
    }
}

s32 Configuration::resolveFilter(Properties const& properties, char const* path) const
{
    try
    {
        s32 filter = std::get<s32>(strToNumber<s32>(properties.property(path, "-1"), path));
        return filter < 0 ? std::numeric_limits<s32>::max() : filter;
    }
    catch ([[maybe_unused]] error::ConversionError const&)
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
        catch (error::PropertyNotFoundError const& e)
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
}  // namespace vfrb::config

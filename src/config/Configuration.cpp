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
#include <utility>

#include "config/ConfigReader.h"
#include "util/Logger.hpp"
#include "util/string_utils.hpp"

using namespace std::literals;

namespace vfrb::config
{
namespace error
{
char const* ConfigurationError::what() const noexcept
{
    return "configuration initialization failed";
}

class ConversionError : public vfrb::error::IError
{
    Str const m_msg;

public:
    ConversionError(str const& str, char const* path) : m_msg("invalid value at "s + path + " [" + str + "]")
    {}
    ~ConversionError() noexcept override = default;

    char const* what() const noexcept override
    {
        return m_msg.c_str();
    }
};
}  // namespace error

/**
 * @brief Convert a string to number.
 * @tparam T    The number type
 * @param str The string to convert
 * @return an optional number, which may be invalid
 */
template<typename T>
T parse(str const& str, char const* path)
{
    try
    {
        return str_util::parse<T>(str);
    }
    catch ([[maybe_unused]] vfrb::str_util::error::CConversionError const&)
    {
        throw error::ConversionError(str, path);
    }
}

constexpr auto     LOG_PREFIX = "(Config) ";
static auto const& logger     = Logger::instance();

Configuration::Configuration(std::istream& stream)
try :
    m_properties(ConfigReader(stream).read()),
    groundMode(m_properties.property(PATH_GND_MODE, "no") == "no"),
    gpsPosition(resolvePosition()),
    atmPressure(parse<f64>(m_properties.property(PATH_PRESSURE, "1013.25"), PATH_PRESSURE)),
    maxHeight(resolveFilter(PATH_MAX_HEIGHT)),
    maxDistance(resolveFilter(PATH_MAX_DIST)),
    serverConfig(
        std::make_tuple(parse<u16>(m_properties.property(PATH_SERVER_PORT, "4353"), PATH_SERVER_PORT),
                        parse<u64>(m_properties.property(PATH_SERVER_MAX_CON, "5"), PATH_SERVER_MAX_CON))),
    feedNames(resolveFeedNames()),
    feedProperties(resolveFeeds())
{
    dumpInfo();
}
catch (vfrb::error::IError const& e)
{
    logger.error(LOG_PREFIX, "init: ", e.what());
    throw error::ConfigurationError();
}

object::GpsPosition Configuration::resolvePosition() const
{
    object::Location pos;
    pos.latitude  = parse<f64>(m_properties.property(PATH_LATITUDE, "0.0"), PATH_LATITUDE);
    pos.longitude = parse<f64>(m_properties.property(PATH_LONGITUDE, "0.0"), PATH_LONGITUDE);
    pos.altitude  = parse<s32>(m_properties.property(PATH_ALTITUDE, "0"), PATH_ALTITUDE);
    f64 geoid     = parse<f64>(m_properties.property(PATH_GEOID, "0.0"), PATH_GEOID);
    return object::GpsPosition(0, pos, geoid);
}

s32 Configuration::resolveFilter(char const* path) const
{
    try
    {
        s32 filter = parse<s32>(m_properties.property(path, "-1"), path);
        return filter < 0 ? std::numeric_limits<s32>::max() : filter;
    }
    catch ([[maybe_unused]] error::ConversionError const&)
    {
        return std::numeric_limits<s32>::max();
    }
}

std::unordered_map<str, Properties> Configuration::resolveFeeds() const
{
    std::unordered_map<str, Properties> map;
    for (auto const& it : feedNames)
    {
        try
        {
            map.emplace(it, m_properties.section(it));
        }
        catch (error::PropertyNotFoundError const& e)
        {
            logger.warn(LOG_PREFIX, "resolving feeds: ", e.what(), " for ", it);
        }
    }
    return map;
}

std::list<str> Configuration::resolveFeedNames() const
{
    std::list<str>    list;
    std::stringstream ss;
    ss.str(m_properties.property(PATH_FEEDS, ""));
    str item;

    while (std::getline(ss, item, ','))
    {
        usize f = item.find_first_not_of(' ');
        if (f != str::npos)
        {
            item = item.substr(f);
        }
        f = item.find_last_not_of(' ');
        if (f != str::npos)
        {
            item = item.substr(0, f + 1);
        }
        list.push_back(item);
    }
    return list;
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
    logger.info(LOG_PREFIX, PATH_SERVER_PORT, ": ", std::get<0>(serverConfig));
    logger.info(LOG_PREFIX, PATH_SERVER_MAX_CON, ": ", std::get<1>(serverConfig));
    logger.info(LOG_PREFIX, PATH_GND_MODE, ": ", groundMode ? "Yes" : "No");
    logger.info(LOG_PREFIX, "number of feeds: ", feedProperties.size());
}
}  // namespace vfrb::config

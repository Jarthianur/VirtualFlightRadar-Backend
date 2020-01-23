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
#include "util/string_utils.hpp"

#include "Logger.hpp"

namespace vfrb::config
{
namespace error
{
char const* CConfigurationError::Message() const noexcept
{
    return "configuration initialization failed";
}

CConversionError::CConversionError(Str const& str_, char const* path_)
    : m_msg(str_util::MakeStr("invalid value at ", path_, " [", str_, "]"))
{}

char const* CConversionError::Message() const noexcept
{
    return m_msg.c_str();
}
}  // namespace error

/// Wrapper for parse function
template<typename T>
T parse(Str const& str_, char const* path_)
{
    try
    {
        return str_util::Parse<T>(str_);
    }
    catch ([[maybe_unused]] vfrb::str_util::error::CConversionError const&)
    {
        throw error::CConversionError(str_, path_);
    }
}

constexpr auto     LOG_PREFIX = "(Config) ";
static auto const& logger     = CLogger::Instance();

CConfiguration::CConfiguration(std::istream& stream_)
try :
    m_properties(CConfigReader(stream_).Read()),
    GroundMode(m_properties.Property(PATH_GND_MODE, "no") == "no"),
    GpsPosition(resolvePosition()),
    AtmPressure(parse<f64>(m_properties.Property(PATH_PRESSURE, "1013.25"), PATH_PRESSURE)),
    MaxHeight(resolveFilter(PATH_MAX_HEIGHT)),
    MaxDistance(resolveFilter(PATH_MAX_DIST)),
    ServerConfig(
        std::make_tuple(parse<u16>(m_properties.Property(PATH_SERVER_PORT, "4353"), PATH_SERVER_PORT),
                        parse<u64>(m_properties.Property(PATH_SERVER_MAX_CON, "5"), PATH_SERVER_MAX_CON))),
    FeedNames(resolveFeedNames()),
    FeedProperties(resolveFeeds())
{
    dumpInfo();
}
catch (vfrb::error::IError const& e)
{
    logger.Error(LOG_PREFIX, "init: ", e.Message());
    throw error::CConfigurationError();
}

object::CGpsPosition CConfiguration::resolvePosition() const
{
    object::SLocation loc;
    loc.Latitude  = parse<f64>(m_properties.Property(PATH_LATITUDE, "0.0"), PATH_LATITUDE);
    loc.Longitude = parse<f64>(m_properties.Property(PATH_LONGITUDE, "0.0"), PATH_LONGITUDE);
    loc.Altitude  = parse<s32>(m_properties.Property(PATH_ALTITUDE, "0"), PATH_ALTITUDE);
    f64 geoid     = parse<f64>(m_properties.Property(PATH_GEOID, "0.0"), PATH_GEOID);
    return object::CGpsPosition(0, loc, geoid);
}

s32 CConfiguration::resolveFilter(char const* path_) const
{
    try
    {
        s32 filter = parse<s32>(m_properties.Property(path_, "-1"), path_);
        return filter < 0 ? std::numeric_limits<s32>::max() : filter;
    }
    catch ([[maybe_unused]] error::CConversionError const&)
    {
        return std::numeric_limits<s32>::max();
    }
}

std::unordered_map<Str, CProperties> CConfiguration::resolveFeeds() const
{
    std::unordered_map<Str, CProperties> map;
    for (auto const& it : FeedNames)
    {
        try
        {
            map.emplace(it, m_properties.Section(it));
        }
        catch (error::CPropertyNotFoundError const& e)
        {
            logger.Warn(LOG_PREFIX, "resolving feeds: ", e.Message(), " for ", it);
        }
    }
    return map;
}

std::list<Str> CConfiguration::resolveFeedNames() const
{
    std::list<Str>    list;
    std::stringstream ss;
    ss.str(m_properties.Property(PATH_FEEDS, ""));
    Str item;

    while (std::getline(ss, item, ','))
    {
        usize f = item.find_first_not_of(' ');
        if (f != Str::npos)
        {
            item = item.substr(f);
        }
        f = item.find_last_not_of(' ');
        if (f != Str::npos)
        {
            item = item.substr(0, f + 1);
        }
        list.push_back(item);
    }
    return list;
}

void CConfiguration::dumpInfo() const
{
    logger.Info(LOG_PREFIX, PATH_LATITUDE, ": ", GpsPosition.Location().Latitude);
    logger.Info(LOG_PREFIX, PATH_LONGITUDE, ": ", GpsPosition.Location().Longitude);
    logger.Info(LOG_PREFIX, PATH_ALTITUDE, ": ", GpsPosition.Location().Altitude);
    logger.Info(LOG_PREFIX, PATH_GEOID, ": ", GpsPosition.Geoid());
    logger.Info(LOG_PREFIX, PATH_PRESSURE, ": ", AtmPressure);
    logger.Info(LOG_PREFIX, PATH_MAX_HEIGHT, ": ", MaxHeight);
    logger.Info(LOG_PREFIX, PATH_MAX_DIST, ": ", MaxDistance);
    logger.Info(LOG_PREFIX, PATH_SERVER_PORT, ": ", std::get<0>(ServerConfig));
    logger.Info(LOG_PREFIX, PATH_SERVER_MAX_CON, ": ", std::get<1>(ServerConfig));
    logger.Info(LOG_PREFIX, PATH_GND_MODE, ": ", GroundMode ? "Yes" : "No");
    logger.Info(LOG_PREFIX, "number of feeds: ", FeedProperties.size());
}
}  // namespace vfrb::config

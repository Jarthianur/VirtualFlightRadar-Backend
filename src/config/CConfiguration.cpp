/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "config/CConfiguration.hpp"

#include <limits>
#include <utility>

#include "config/CConfigReader.hpp"
#include "util/StringUtils.hpp"

#include "CLogger.hpp"

namespace vfrb::config
{
namespace error
{
auto
CConfigurationError::Message() const noexcept -> str {
    return "configuration initialization failed";
}

CConversionError::CConversionError(String const& str_, str path_)
    : m_msg(str_util::MakeString("invalid value at ", path_, " [", str_, "]")) {}

auto
CConversionError::Message() const noexcept -> str {
    return m_msg.c_str();
}
}  // namespace error

template<typename T>
auto
parse(String const& str_, str path_) -> T {
    try {
        return str_util::Parse<T>(str_);
    } catch ([[maybe_unused]] vfrb::str_util::error::CConversionError const&) {
        throw error::CConversionError(str_, path_);
    }
}

CTCONST            LOG_PREFIX = "(Config) ";
static auto const& logger     = CLogger::Instance();

CConfiguration::CConfiguration(std::istream& stream_) try
    : m_properties(CConfigReader(stream_).Read()),
      GroundMode(m_properties.Property(PATH_GND_MODE, "no") != "no"),
      GpsPosition(resolvePosition()),
      AtmPressure(parse<f64>(m_properties.Property(PATH_PRESSURE, "1013.25"), PATH_PRESSURE)),
      MaxHeight(resolveFilter(PATH_MAX_HEIGHT)),
      MaxDistance(resolveFilter(PATH_MAX_DIST)),
      ServerConfig({parse<u16>(m_properties.Property(PATH_SERVER_PORT, "4353"), PATH_SERVER_PORT),
                    parse<u64>(m_properties.Property(PATH_SERVER_MAX_CON, "1024"), PATH_SERVER_MAX_CON)}),
      FeedNames(resolveFeedNames()),
      FeedProperties(resolveFeeds()) {
    dumpInfo();
} catch (vfrb::error::IError const& e) {
    logger.Error(LOG_PREFIX, "init: ", e.Message());
    throw error::CConfigurationError();
}

auto
CConfiguration::resolvePosition() const -> object::CGpsPosition {
    object::SLocation loc{};
    loc.Latitude  = parse<f64>(m_properties.Property(PATH_LATITUDE, "0.0"), PATH_LATITUDE);
    loc.Longitude = parse<f64>(m_properties.Property(PATH_LONGITUDE, "0.0"), PATH_LONGITUDE);
    loc.Altitude  = parse<i32>(m_properties.Property(PATH_ALTITUDE, "0"), PATH_ALTITUDE);
    f64 geoid     = parse<f64>(m_properties.Property(PATH_GEOID, "0.0"), PATH_GEOID);
    return object::CGpsPosition(0, loc, geoid, 1.0, 3, 5, object::CTimestamp());
}

auto
CConfiguration::resolveFilter(str key_) const -> i32 {
    try {
        i32 filter = parse<i32>(m_properties.Property(key_, "-1"), key_);
        return filter < 0 ? limits::i32::max() : filter;
    } catch ([[maybe_unused]] error::CConversionError const&) {
        return limits::i32::max();
    }
}

auto
CConfiguration::resolveFeeds() const -> HashMap<String, CProperties> {
    HashMap<String, CProperties> map;
    for (auto const& it : FeedNames) {
        try {
            map.emplace(it, m_properties.Section(it));
        } catch (error::CPropertyNotFoundError const& e) {
            logger.Warn(LOG_PREFIX, "resolving feeds: ", e.Message(), " for ", it);
        }
    }
    return map;
}

auto
CConfiguration::resolveFeedNames() const -> List<String> {
    List<String>      list;
    std::stringstream ss;
    ss.str(m_properties.Property(PATH_FEEDS, ""));
    String item;

    while (std::getline(ss, item, ',')) {
        usize f = item.find_first_not_of(' ');
        if (f != String::npos) {
            item = item.substr(f);
        }
        f = item.find_last_not_of(' ');
        if (f != String::npos) {
            item = item.substr(0, f + 1);
        }
        list.push_back(item);
    }
    return list;
}

void
CConfiguration::dumpInfo() const {
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

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

#pragma once

#include <cstdint>
#include <istream>
#include <list>
#include <string>

#include "object/GpsPosition.h"

#include "Properties.h"

namespace config
{
/**
 * @brief VFRB Configuration
 */
class Configuration final
{
    //< begin members >//
    const Properties m_properties;  ///< Properties from file
    //< end members >//

    //< begin methods >//
    /**
     * @brief Resolve the fallback position.
     * @param properties The properties
     * @return the position
     */
    object::GpsPosition resolvePosition(const Properties& properties) const;

    /**
     * @brief Resolve the server port.
     * @param properties The properties
     * @return the port number
     */
    std::uint16_t resolveServerPort(const Properties& properties) const;

    /**
     * @brief Resolve the feeds and their config.
     * @param properties The properties
     */
    std::unordered_map<std::string, Properties> resolveFeeds(const Properties& properties);

    /**
     * @brief Resolve a filter value.
     * @note An invalid/negative value results in the max value which means disabled.
     * @param properties The properties
     * @param path       The filter key
     * @return the filter value
     */
    std::int32_t resolveFilter(const Properties& properties, const char* key) const;

    /**
     * @brief Dump the current config state as info log.
     */
    void dumpInfo() const;
    //< end methods >//

    //< begin constants >//
    static constexpr auto LOG_PREFIX = "(Config) ";

    static constexpr auto PATH_FEEDS       = "general.feeds";
    static constexpr auto PATH_GND_MODE    = "general.gndMode";
    static constexpr auto PATH_SERVER_PORT = "general.serverPort";
    static constexpr auto PATH_LATITUDE    = "fallback.latitude";
    static constexpr auto PATH_LONGITUDE   = "fallback.longitude";
    static constexpr auto PATH_ALTITUDE    = "fallback.altitude";
    static constexpr auto PATH_GEOID       = "fallback.geoid";
    static constexpr auto PATH_PRESSURE    = "fallback.pressure";
    static constexpr auto PATH_MAX_DIST    = "filter.maxDistance";
    static constexpr auto PATH_MAX_HEIGHT  = "filter.maxHeight";
    //< end constants >//

public:
    //< begin members >//
    bool                         groundMode;   ///< Ground mode state
    const object::GpsPosition    gpsPosition;  ///< Fallback position
    const double                 atmPressure;  ///< Atmospheric fallback pressure
    const std::int32_t           maxHeight;    ///< Maximum height for reported aircrafts
    const std::int32_t           maxDistance;  ///< Maximum distance for reported aircrafts
    const std::uint16_t          serverPort;   ///< Port where to serve reports
    const std::list<std::string> feedNames;    ///< List of feed names
    const std::unordered_map<std::string, Properties> feedProperties;  ///< Map feed names to their properties
    //< end members >//

    /**
     * @param stream The input stream
     * @throw std::logic_error if any error occurres
     */
    explicit Configuration(std::istream& stream);
    ~Configuration() noexcept = default;

    //< begin constants >//
    // Configuration section keys
    static constexpr auto SECT_KEY_FALLBACK = "fallback";
    static constexpr auto SECT_KEY_GENERAL  = "general";
    static constexpr auto SECT_KEY_FILTER   = "filter";

    // Keywords for feeds
    static constexpr auto SECT_KEY_APRSC = "aprs";
    static constexpr auto SECT_KEY_SBS   = "sbs";
    static constexpr auto SECT_KEY_GPS   = "gps";
    static constexpr auto SECT_KEY_WIND  = "wind";
    static constexpr auto SECT_KEY_ATMOS = "atm";

    // Property keys for section "general"
    static constexpr auto KV_KEY_FEEDS       = "feeds";
    static constexpr auto KV_KEY_GND_MODE    = "gndMode";
    static constexpr auto KV_KEY_SERVER_PORT = "serverPort";

    // Property keys for section "fallback"
    static constexpr auto KV_KEY_LATITUDE  = "latitude";
    static constexpr auto KV_KEY_LONGITUDE = "longitude";
    static constexpr auto KV_KEY_ALTITUDE  = "altitude";
    static constexpr auto KV_KEY_GEOID     = "geoid";
    static constexpr auto KV_KEY_PRESSURE  = "pressure";

    // Property keys for section "filter"
    static constexpr auto KV_KEY_MAX_DIST   = "maxDistance";
    static constexpr auto KV_KEY_MAX_HEIGHT = "maxHeight";

    // Property keys for feed sections
    static constexpr auto KV_KEY_HOST     = "host";
    static constexpr auto KV_KEY_PORT     = "port";
    static constexpr auto KV_KEY_PRIORITY = "priority";
    static constexpr auto KV_KEY_LOGIN    = "login";
    //< end constants >//
};

}  // namespace config

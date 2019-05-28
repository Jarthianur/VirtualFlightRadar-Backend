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
#include "util/defines.h"

#include "Properties.h"

namespace config
{
/**
 * @brief VFRB Configuration
 */
class Configuration
{
    //< begin members >//
    /// Fallback position
    object::GpsPosition m_position;

    /// Atmospheric fallback pressure
    double m_atmPressure;

    /// Maximum height for reported aircrafts
    std::int32_t m_maxHeight;

    /// Maximum distance for reported aircrafts
    std::int32_t m_maxDistance;

    /// Port where to serve reports
    std::uint16_t m_serverPort;

    /// Ground mode state
    bool m_groundMode;

    /// List of feed names
    std::list<std::string> m_feedNames;

    ///  Map feed names to their properties
    std::unordered_map<std::string, Properties> m_feedProperties;
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
    void resolveFeeds(const Properties& properties);

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
    CONST_LITERAL LOG_PREFIX = "(Config) ";

    // Path definitions
    CONST_LITERAL PATH_FEEDS       = "general.feeds";
    CONST_LITERAL PATH_GND_MODE    = "general.gndMode";
    CONST_LITERAL PATH_SERVER_PORT = "general.serverPort";
    CONST_LITERAL PATH_LATITUDE    = "fallback.latitude";
    CONST_LITERAL PATH_LONGITUDE   = "fallback.longitude";
    CONST_LITERAL PATH_ALTITUDE    = "fallback.altitude";
    CONST_LITERAL PATH_GEOID       = "fallback.geoid";
    CONST_LITERAL PATH_PRESSURE    = "fallback.pressure";
    CONST_LITERAL PATH_MAX_DIST    = "filter.maxDistance";
    CONST_LITERAL PATH_MAX_HEIGHT  = "filter.maxHeight";
    //< end constants >//

public:
    //< begin ctors/dtors >//
    /**
     * @brief Constructor
     * @param stream The input stream
     * @throw std::logic_error if any error occurres
     */
    explicit Configuration(std::istream& stream);

    DEFAULT_DTOR(Configuration)
    //< end ctors/dtors >//

    //< begin getters/setters >//
    GETTER_CR(position)
    GETTER_V(atmPressure)
    GETTER_V(maxHeight)
    GETTER_V(maxDistance)
    GETTER_V(serverPort)
    GETSET_V(groundMode)
    GETTER_CR(feedNames)
    GETTER_CR(feedProperties)
    //< end getters/setters >//

    //< begin constants >//
    // Configuration section keys
    CONST_LITERAL SECT_KEY_FALLBACK = "fallback";
    CONST_LITERAL SECT_KEY_GENERAL  = "general";
    CONST_LITERAL SECT_KEY_FILTER   = "filter";

    // Keywords for feeds
    CONST_LITERAL SECT_KEY_APRSC = "aprs";
    CONST_LITERAL SECT_KEY_SBS   = "sbs";
    CONST_LITERAL SECT_KEY_GPS   = "gps";
    CONST_LITERAL SECT_KEY_WIND  = "wind";
    CONST_LITERAL SECT_KEY_ATMOS = "atm";

    // Property keys for section "general"
    CONST_LITERAL KV_KEY_FEEDS       = "feeds";
    CONST_LITERAL KV_KEY_GND_MODE    = "gndMode";
    CONST_LITERAL KV_KEY_SERVER_PORT = "serverPort";

    // Property keys for section "fallback"
    CONST_LITERAL KV_KEY_LATITUDE  = "latitude";
    CONST_LITERAL KV_KEY_LONGITUDE = "longitude";
    CONST_LITERAL KV_KEY_ALTITUDE  = "altitude";
    CONST_LITERAL KV_KEY_GEOID     = "geoid";
    CONST_LITERAL KV_KEY_PRESSURE  = "pressure";

    // Property keys for section "filter"
    CONST_LITERAL KV_KEY_MAX_DIST   = "maxDistance";
    CONST_LITERAL KV_KEY_MAX_HEIGHT = "maxHeight";

    // Property keys for feed sections
    CONST_LITERAL KV_KEY_HOST     = "host";
    CONST_LITERAL KV_KEY_PORT     = "port";
    CONST_LITERAL KV_KEY_PRIORITY = "priority";
    CONST_LITERAL KV_KEY_LOGIN    = "login";
    //< end constants >//
};

}  // namespace config

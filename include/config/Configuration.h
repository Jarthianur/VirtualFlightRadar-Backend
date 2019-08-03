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
    const Properties m_properties;  ///< Properties from file

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

public:
    // Configuration section keys
    static const char* SECT_KEY_FALLBACK;
    static const char* SECT_KEY_GENERAL;
    static const char* SECT_KEY_FILTER;

    // Keywords for feeds
    static const char* SECT_KEY_APRSC;
    static const char* SECT_KEY_SBS;
    static const char* SECT_KEY_GPS;
    static const char* SECT_KEY_WIND;
    static const char* SECT_KEY_ATMOS;

    // Property keys for section "general"
    static const char* KV_KEY_FEEDS;
    static const char* KV_KEY_GND_MODE;
    static const char* KV_KEY_SERVER_PORT;

    // Property keys for section "fallback"
    static const char* KV_KEY_LATITUDE;
    static const char* KV_KEY_LONGITUDE;
    static const char* KV_KEY_ALTITUDE;
    static const char* KV_KEY_GEOID;
    static const char* KV_KEY_PRESSURE;

    // Property keys for section "filter"
    static const char* KV_KEY_MAX_DIST;
    static const char* KV_KEY_MAX_HEIGHT;

    // Property keys for feed sections
    static const char* KV_KEY_HOST;
    static const char* KV_KEY_PORT;
    static const char* KV_KEY_PRIORITY;
    static const char* KV_KEY_LOGIN;

    static const char* PATH_FEEDS;
    static const char* PATH_GND_MODE;
    static const char* PATH_SERVER_PORT;
    static const char* PATH_LATITUDE;
    static const char* PATH_LONGITUDE;
    static const char* PATH_ALTITUDE;
    static const char* PATH_GEOID;
    static const char* PATH_PRESSURE;
    static const char* PATH_MAX_DIST;
    static const char* PATH_MAX_HEIGHT;

    bool                         groundMode;   ///< Ground mode state
    const object::GpsPosition    gpsPosition;  ///< Fallback position
    const double                 atmPressure;  ///< Atmospheric fallback pressure
    const std::int32_t           maxHeight;    ///< Maximum height for reported aircrafts
    const std::int32_t           maxDistance;  ///< Maximum distance for reported aircrafts
    const std::uint16_t          serverPort;   ///< Port where to serve reports
    const std::list<std::string> feedNames;    ///< List of feed names
    const std::unordered_map<std::string, Properties> feedProperties;  ///< Map feed names to their properties

    /**
     * @param stream The input stream
     * @throw std::logic_error if any error occurres
     */
    explicit Configuration(std::istream& stream);
    ~Configuration() noexcept = default;
};

}  // namespace config

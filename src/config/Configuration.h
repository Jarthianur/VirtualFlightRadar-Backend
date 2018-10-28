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

#include "../object/GpsPosition.h"
#include "../util/defines.h"
#include "../util/utility.hpp"

#include "Properties.h"

/**
 * Configuration section keys
 */
#define SECT_KEY_FALLBACK "fallback"
#define SECT_KEY_GENERAL "general"
#define SECT_KEY_FILTER "filter"

/**
 * Keywords for feeds
 */
#define SECT_KEY_APRSC "aprs"
#define SECT_KEY_SBS "sbs"
#define SECT_KEY_GPS "gps"
#define SECT_KEY_WIND "wind"
#define SECT_KEY_ATMOS "atm"

/**
 * Property keys for section "general"
 */
#define KV_KEY_FEEDS "feeds"
#define KV_KEY_GND_MODE "gndMode"
#define KV_KEY_SERVER_PORT "serverPort"

/**
 * Property keys for section "fallback"
 */
#define KV_KEY_LATITUDE "latitude"
#define KV_KEY_LONGITUDE "longitude"
#define KV_KEY_ALTITUDE "altitude"
#define KV_KEY_GEOID "geoid"
#define KV_KEY_PRESSURE "pressure"

/**
 * Property keys for section "filter"
 */
#define KV_KEY_MAX_DIST "maxDist"
#define KV_KEY_MAX_HEIGHT "maxHeight"

/**
 * Property keys for feed sections
 */
#define KV_KEY_HOST "host"
#define KV_KEY_PORT "port"
#define KV_KEY_PRIORITY "priority"
#define KV_KEY_LOGIN "login"

/// Concat section and key
#define PATH(S, K) (S "." K)

namespace config
{
/**
 * Path definitions
 */
constexpr const char* PATH_FEEDS       = PATH(SECT_KEY_GENERAL, KV_KEY_FEEDS);
constexpr const char* PATH_GND_MODE    = PATH(SECT_KEY_GENERAL, KV_KEY_GND_MODE);
constexpr const char* PATH_SERVER_PORT = PATH(SECT_KEY_GENERAL, KV_KEY_SERVER_PORT);
constexpr const char* PATH_LATITUDE    = PATH(SECT_KEY_FALLBACK, KV_KEY_LATITUDE);
constexpr const char* PATH_LONGITUDE   = PATH(SECT_KEY_FALLBACK, KV_KEY_LONGITUDE);
constexpr const char* PATH_ALTITUDE    = PATH(SECT_KEY_FALLBACK, KV_KEY_ALTITUDE);
constexpr const char* PATH_GEOID       = PATH(SECT_KEY_FALLBACK, KV_KEY_GEOID);
constexpr const char* PATH_PRESSURE    = PATH(SECT_KEY_FALLBACK, KV_KEY_PRESSURE);
constexpr const char* PATH_MAX_DIST    = PATH(SECT_KEY_FILTER, KV_KEY_MAX_DIST);
constexpr const char* PATH_MAX_HEIGHT  = PATH(SECT_KEY_FILTER, KV_KEY_MAX_HEIGHT);

/**
 * @brief VFRB Configuration
 */
class Configuration
{
public:
    /**
     * @brief Constructor
     * @param stream The input stream
     * @throw std::logic_error if any error occurres
     */
    explicit Configuration(std::istream& stream);

    /**
     * @brief Destructor
     */
    ~Configuration() noexcept = default;

private:
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
    std::int32_t resolveFilter(const Properties& properties, const std::string& key) const;

    /**
     * @brief Check an optional Number to be valid.
     * @param number The optinonal Number
     * @param path   The key path
     * @return the number value
     * @throw std::invalid_argument if the Number is invalid
     */
    util::Number checkNumber(const util::OptNumber& number, const std::string& path) const;

    /**
     * @brief Dump the current config state as info log.
     */
    void dumpInfo() const;

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

public:
    /**
     * Getters
     */
    GETTER_CR(position)
    GETTER_V(atmPressure)
    GETTER_V(maxHeight)
    GETTER_V(maxDistance)
    GETTER_V(serverPort)
    GETSET_V(groundMode)
    GETTER_CR(feedNames)
    GETTER_CR(feedProperties)
};

}  // namespace config

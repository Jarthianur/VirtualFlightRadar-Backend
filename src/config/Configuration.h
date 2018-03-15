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
#include <string>
#include <utility>

#include "../Defines.h"
#include "../object/Position.h"
#include "PropertyMap.h"
#include "Util.hpp"

/**
 * Configuration section keys
 */
/// @def SECT_KEY_FALLBACK
#define SECT_KEY_FALLBACK "fallback"

/// @def SECT_KEY_GENERAL
#define SECT_KEY_GENERAL "general"

/// @def SECT_KEY_FILTER
#define SECT_KEY_FILTER "filter"

/// @def SECT_KEY_APRSC
#define SECT_KEY_APRSC "aprs"

/// @def SECT_KEY_SBS
#define SECT_KEY_SBS "sbs"

/// @def SECT_KEY_GPS
#define SECT_KEY_GPS "gps"

/// @def SECT_KEY_SENS
#define SECT_KEY_SENS "sens"

/**
 * Per section key-value keys
 */
/// @def KV_KEY_FEEDS
#define KV_KEY_FEEDS "feeds"

/// @def KV_KEY_GND_MODE
#define KV_KEY_GND_MODE "gndMode"

/// @def KV_KEY_LATITUDE
#define KV_KEY_LATITUDE "latitude"

/// @def KV_KEY_LONGITUDE
#define KV_KEY_LONGITUDE "longitude"

/// @def KV_KEY_ALTITUDE
#define KV_KEY_ALTITUDE "altitude"

/// @def KV_KEY_GEOID
#define KV_KEY_GEOID "geoid"

/// @def KV_KEY_PRESSURE
#define KV_KEY_PRESSURE "pressure"

/// @def KV_KEY_TEMPERATURE
#define KV_KEY_TEMPERATURE "temperature"

/// @def KV_KEY_MAX_DIST
#define KV_KEY_MAX_DIST "maxDist"

/// @def KV_KEY_MAX_HEIGHT
#define KV_KEY_MAX_HEIGHT "maxHeight"

/// @def KV_KEY_SERVER_PORT
#define KV_KEY_SERVER_PORT "serverPort"

/// @def KV_KEY_HOST
#define KV_KEY_HOST "host"

/// @def KV_KEY_PORT
#define KV_KEY_PORT "port"

/// @def KV_KEY_PRIORITY
#define KV_KEY_PRIORITY "priority"

/// @def KV_KEY_LOGIN
#define KV_KEY_LOGIN "login"

/// @namespace config
namespace config
{
/// @typedef FeedMapping
/// List of pairs with feed name and key-values
using FeedMapping = std::list<std::pair<std::string, KeyValueMap>>;

/**
 * @class Configuration
 * @brief Represents a VFRB configuration.
 *
 * Evaluate and store a configuration for the VFRB.
 */
class Configuration
{
public:
    /**
     * @fn Configuration
     * @brief Constructor
     * @param rStream The input stream
     * @throw std::runtime_error if any error occures
     */
    explicit Configuration(std::istream& rStream);

    /**
     * @fn ~Configuration
     * @brief Destructor
     */
    virtual ~Configuration() noexcept;

    /**
     * Define and declare getters.
     */
    GETTER_CR(object::GpsPosition, mPosition, Position)
    GETTER_V(double, mAtmPressure, AtmPressure)
    GETTER_V(std::int32_t, mMaxHeight, MaxHeight)
    GETTER_V(std::int32_t, mMaxDistance, MaxDistance)
    GETTER_V(std::uint16_t, mServerPort, ServerPort)
    GETTER_CR(FeedMapping, mFeedMapping, FeedMapping)

    /**
     * @fn isGndModeEnabled
     * @brief Is the ground mode enabled?
     * @return true if enabled, else false
     */
    bool isGndModeEnabled() const;

    /**
     * @fn forceGndMode
     * @brief Force ground mode to be enabled.
     */
    void forceGndMode();

private:
    /**
     * @fn init
     * @brief Unpack the given properties.
     * @param crProperties The properties
     * @throw std::invalid_argument from invoked statements
     */
    void init(const PropertyMap& crProperties);

    /**
     * @fn resolvePosition
     * @brief Resolve the fallback position.
     * @param crProperties The properties
     * @return the position
     */
    object::GpsPosition resolvePosition(const PropertyMap& crProperties) const;

    /**
     * @fn resolveServerPort
     * @brief Resolve the server port.
     * @param crProperties The properties
     * @return the port
     */
    std::uint16_t resolveServerPort(const PropertyMap& crProperties) const;

    /**
     * @fn resolveFeeds
     * @brief Resolve the feeds and their config.
     * @param crProperties The properties
     * @return a list of all feeds with their sections
     */
    FeedMapping resolveFeeds(const PropertyMap& crProperties);

    /**
     * @fn resolveFilter
     * @brief Resolve a filter.
     * @note An invalid/negative value results in the max value which means disabled.
     * @param crProperties The properties
     * @param crKey        The filter key
     * @return the filter value
     */
    std::int32_t resolveFilter(const PropertyMap& crProperties,
                               const std::string& crKey) const;

    /**
     * @fn checkNumberValue
     * @brief Check a Number.
     * @param crOptNumber The optinonal Number
     * @param crSection   The section name
     * @param crKey       The key
     * @return the number value
     * @throw std::invalid_argument if the Number is invalid
     */
    util::Number checkNumberValue(const util::OptNumber& crOptNumber,
                                  const std::string& crSection,
                                  const std::string& crKey) const;

    /**
     * @fn dumpInfo
     * @brief Dump the current config state using info log.
     */
    void dumpInfo() const;

    /// @var mPosition
    /// Fallback position
    object::GpsPosition mPosition;

    /// @var mAtmPressure
    /// Atmospheric fallback pressure
    double mAtmPressure;

    /// @var mMaxHeight
    /// Maximum height for reported aircrafts
    std::int32_t mMaxHeight;

    /// @var mMaxDistance
    /// Maximum distance for reported aircrafts
    std::int32_t mMaxDistance;

    /// @var mServerPort
    /// Port where to serve reports
    std::uint16_t mServerPort;

    /// @var mFeedMapping
    /// List of feeds with their key-value map
    FeedMapping mFeedMapping;
};

}  // namespace config

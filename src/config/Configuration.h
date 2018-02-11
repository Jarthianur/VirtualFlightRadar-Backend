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

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>

namespace feed
{
class Feed;
}

/// @namespace config
namespace config
{
class PropertyMap;

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
 * Per section keys
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

/**
 * @class Configuration
 * @brief Evaluate and store configuration for VFR-B.
 */
class Configuration
{
public:
    /**
     * @fn Configuration
     * @brief Constructor
     * @param rStream The config file as stream
     * @throw std::logic_error if any error occures or no feeds are given
     */
    explicit Configuration(std::istream& rStream);

    /**
     * @fn ~Configuration
     * @brief Destructor
     */
    virtual ~Configuration() noexcept;

    /// @var sBaseAltitude
    /// Base altitude
    static std::int32_t sBaseAltitude;

    /// @var sBaseLatitude
    /// Base latitude
    static double sBaseLatitude;

    /// @var sBaseLongitude
    /// Base longitude
    static double sBaseLongitude;

    /// @var sBaseGeoid
    /// Base geoid separation
    static double sBaseGeoid;

    /// @var sBaseAtmPressure
    /// Atmospheric pressure at base
    static double sBaseAtmPressure;

    /// @var sMaxHeight
    /// Maximum height for reported Aircrafts
    static std::int32_t sMaxHeight;

    /// @var sMaxDistance
    /// Maximum distance for reported Aircrafts
    static std::int32_t sMaxDistance;

    /// @var sServerPort
    /// Port where to serve reports
    static std::uint16_t sServerPort;

    /// @var sGndModeEnabled
    /// Ground mode enabled?
    static bool sGndModeEnabled;

    /// @var sRegisteredFeeds
    /// All registered and correctly parsed input feeds
    static std::vector<std::shared_ptr<feed::Feed>> sRegisteredFeeds;

private:
    /**
     * @fn init
     * @brief Initialize Configuration, read and unpack the config stream.
     * @param rStream The input stream
     * @return true on success and at least one feed was registered, else false
     */
    bool init(std::istream& rStream);

    /**
     * @fn registerFeeds
     * @brief Register all input feeds found from ConfigReader.
     * @note Only correctly configured feeds get registered.
     * @param crProperties The PropertyMap holding read properties
     * @return the number of registered feeds
     */
    std::size_t registerFeeds(const PropertyMap& crProperties);

    /**
     * @fn resolveFallbacks
     * @brief Resolve all fallback values from the properties.
     * @param crProperties The properties
     * @return whether all fallbacks could be resolved
     */
    bool resolveFallbacks(const PropertyMap& crProperties);

    /**
     * @fn resolveFilter
     * @brief Resolve a filter from the properties.
     * @param crProperties The properties
     * @param crKey The filter key
     * @return the filter value
     */
    std::int32_t resolveFilter(const PropertyMap& crProperties, const std::string& crKey);

    /**
     * @fn resolveNumberKey
     * @brief Resolve a number value for a key.
     * @tparam T        The type of number
     * @param crProperties     The properties
     * @param crSection The section name
     * @param crKey     The key
     * @param crDefault The default value
     * @return the resolved number value
     *
     * Maybe use a union to prevent specialization.
     */
    template<typename T>
    T resolveNumberValue(const PropertyMap& crProperties, const std::string& crSection,
                         const std::string& crKey, const std::string& crDefault);

    /**
     * @fn resolveServerPort
     * @brief Resolve the server port.
     * @param crPort The port string
     * @return the port
     */
    std::uint16_t resolveServerPort(const std::string& crPort);

    /**
     * @fn resolveFeeds
     * @brief Resolve all Feed names.
     * @param crFeeds The feeds string
     * @return all feed names
     */
    std::vector<std::string> resolveFeeds(const std::string& crFeeds);
};

}  // namespace config

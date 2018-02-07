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

#ifndef SRC_CONFIG_CONFIGURATION_H_
#define SRC_CONFIG_CONFIGURATION_H_

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

namespace config
{

class PropertyMap;

/// Configuration section keys
#define SECT_KEY_FALLBACK   "fallback"
#define SECT_KEY_GENERAL    "general"
#define SECT_KEY_FILTER     "filter"
#define SECT_KEY_APRSC      "aprs"
#define SECT_KEY_SBS        "sbs"
#define SECT_KEY_GPS        "gps"
#define SECT_KEY_SENS       "sens"
/// Per section keys
#define KV_KEY_FEEDS        "feeds"
#define KV_KEY_GND_MODE     "gndMode"
#define KV_KEY_LATITUDE     "latitude"
#define KV_KEY_LONGITUDE    "longitude"
#define KV_KEY_ALTITUDE     "altitude"
#define KV_KEY_GEOID        "geoid"
#define KV_KEY_PRESSURE     "pressure"
#define KV_KEY_TEMPERATURE  "temperature"
#define KV_KEY_MAX_DIST     "maxDist"
#define KV_KEY_MAX_HEIGHT   "maxHeight"
#define KV_KEY_SERVER_PORT  "serverPort"
#define KV_KEY_HOST         "host"
#define KV_KEY_PORT         "port"
#define KV_KEY_PRIORITY     "priority"
#define KV_KEY_LOGIN        "login"

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
     * @param r_stream The config file as stream
     * @throws std::logic_error if any error occures or no feeds are given
     */
    Configuration(std::istream& r_stream);
    /**
     * @fn ~Configuration
     * @brief Destructor
     */
    virtual ~Configuration() noexcept;

    /// Bases altitude
    static std::int32_t base_altitude;
    /// Bases latitude
    static double base_latitude;
    /// Bases longitude
    static double base_longitude;
    /// Bases geoid separation
    static double base_geoid;
    /// Air pressure at base
    static double base_pressure;
    /// Maximum height for reported Aircrafts
    static std::int32_t filter_maxHeight;
    /// Maximum distance for reported Aircrafts
    static std::int32_t filter_maxDist;
    /// Port where to serve reports
    static std::uint16_t global_server_port;
    /// Ground mode enabled?
    static bool global_gnd_mode;
    /// All registered and correctly parsed input feeds
    static std::vector<std::shared_ptr<feed::Feed>> global_feeds;

private:
    /**
     * @fn init
     * @brief Initialize Configuration, read and unpack config file with ConfigReader.
     * @param r_stream The input stream
     * @return true on success and at least one feed was registered, else false
     */
    bool init(std::istream& r_stream);
    /**
     * @fn registerFeeds
     * @brief Register all input feeds found from ConfigReader.
     * @note Only correctly configured feeds get registered.
     * @param cr_map The PropertyMap holding read properties
     * @return the number of registered feeds
     */
    std::size_t registerFeeds(const PropertyMap& cr_map);

    void resolveFallbacks(const PropertyMap& cr_map);
    void resolveFilters(const PropertyMap& cr_map);
};

}  // namespace config

#endif /* SRC_CONFIG_CONFIGURATION_H_ */

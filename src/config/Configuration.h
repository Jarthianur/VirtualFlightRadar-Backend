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
#include <string>
#include <vector>

#include "../vfrb/Feed.h"
#include "ConfigReader.h"

namespace config
{

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
 * The Configuration class.
 *
 * This class provides functionality to unpack properties
 * read by the ConfigReader into necessary information for
 * running the VFR-B.
 */
class Configuration
{
public:
    /**
     * Constructor to initialize all Configuration fields.
     *
     * @param file the config file
     */
    Configuration(const char* /*file*/);
    /**
     * Destructor
     *
     * @exceptsafe no-throw
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
    static std::vector<vfrb::Feed> global_feeds;

private:
    /**
     * Initialize Configuration.
     * Called by c'tor.
     * Read and unpack config file.
     *
     * @param file the file name
     *
     * @return whether reading and unpacking was successful
     */
    bool init(const char* /*file*/);
    /**
     * Register all input feeds found in the config file.
     * Only correctly configured feeds are registered.
     *
     * @param r_cr the ConfigReader holding read properties
     *
     * @return the number of registered feeds
     */
    std::size_t registerFeeds(ConfigReader& /*r_cr*/);
    /**
     * Parse a string to integer.
     * Always returns a valid value.
     *
     * @param cr_str the string to parse
     *
     * @return the parsed number, 0 if error
     *
     * @exceptsafe no-throw
     */
    std::int32_t strToInt(const std::string& /*cr_str*/) noexcept;
    /**
     * Parse a string to double.
     * Always returns a valid value.
     *
     * @param cr_str the string to parse
     *
     * @return the parsed number, 0 if error
     *
     * @exceptsafe no-throw
     */
    double strToDouble(const std::string& /*cr_str*/) noexcept;
};

}  // namespace config

#endif /* SRC_CONFIG_CONFIGURATION_H_ */

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

// Keywords for configuration
#define SECT_KEY_FALLBACK  "fallback"
#define SECT_KEY_GENERAL   "general"
#define SECT_KEY_FILTER    "filter"
#define SECT_KEY_APRSC     "aprsc"
#define SECT_KEY_SBS       "sbs"
#define SECT_KEY_GPS       "gps"
#define SECT_KEY_SENS      "sens"
#define KV_KEY_FEEDS       "feeds"
#define KV_KEY_LATITUDE    "latitude"
#define KV_KEY_LONGITUDE   "longitude"
#define KV_KEY_ALTITUDE    "altitude"
#define KV_KEY_GEOID       "geoid"
#define KV_KEY_PRESSURE    "pressure"
#define KV_KEY_TEMPERATURE "temperature"
#define KV_KEY_MAX_DIST    "maxDist"
#define KV_KEY_MAX_HEIGHT  "maxHeight"
#define KV_KEY_SERVER_PORT "serverPort"
#define KV_KEY_HOST        "host"
#define KV_KEY_PORT        "port"
#define KV_KEY_PRIORITY    "priority"
#define KV_KEY_LOGIN       "login"

class Configuration
{
public:
    Configuration(const char* /*file*/);
    virtual ~Configuration() noexcept;

    static std::int32_t base_altitude;
    static double base_latitude;
    static double base_longitude;
    static double base_geoid;
    static double base_pressure;
    static double base_temp;
    static std::int32_t filter_maxHeight;
    static std::int32_t filter_maxDist;
    static std::uint16_t global_server_port;
    static std::vector<Feed> global_feeds;

private:
    bool init(const char* /*file*/);
    std::size_t registerFeeds(ConfigReader& /*cr*/);
    std::int32_t strToInt(const std::string& /*str*/) noexcept;
    double strToDouble(const std::string& /*str*/) noexcept;
};

#endif /* SRC_CONFIG_CONFIGURATION_H_ */

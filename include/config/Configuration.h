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

#include <istream>
#include <list>
#include <tuple>

#include "error/Error.hpp"
#include "object/GpsPosition.h"

#include "Properties.h"
#include "types.h"

namespace vfrb::config
{
/**
 * @brief VFRB Configuration
 */
class CConfiguration final
{
    CProperties const m_properties;  ///< Properties from file

    /**
     * @brief Resolve the fallback position.
     * @param properties The properties
     * @return the position
     */
    object::CGpsPosition resolvePosition() const;

    /**
     * @brief Resolve the feeds and their config.
     * @param properties The properties
     */
    std::unordered_map<Str, CProperties> resolveFeeds() const;

    std::list<Str> resolveFeedNames() const;

    /**
     * @brief Resolve a filter value.
     * @note An invalid/negative value results in the max value which means disabled.
     * @param properties The properties
     * @param path       The filter key
     * @return the filter value
     */
    s32 resolveFilter(char const* key_) const;

    /**
     * @brief Dump the current config state as info log.
     */
    void dumpInfo() const;

public:
    // Configuration section keys
    inline static constexpr auto SECT_KEY_FALLBACK = "fallback";
    inline static constexpr auto SECT_KEY_GENERAL  = "general";
    inline static constexpr auto SECT_KEY_FILTER   = "filter";
    inline static constexpr auto SECT_KEY_SERVER   = "server";

    // Keywords for feeds
    inline static constexpr auto SECT_KEY_APRSC = "aprs";
    inline static constexpr auto SECT_KEY_SBS   = "sbs";
    inline static constexpr auto SECT_KEY_GPS   = "gps";
    inline static constexpr auto SECT_KEY_WIND  = "wind";
    inline static constexpr auto SECT_KEY_ATMOS = "atm";

    // Property keys for section "general"
    inline static constexpr auto KV_KEY_FEEDS    = "feeds";
    inline static constexpr auto KV_KEY_GND_MODE = "gndMode";

    // Property keys for section "server"
    inline static constexpr auto KV_KEY_MAX_CON = "maxConnections";

    // Property keys for section "fallback"
    inline static constexpr auto KV_KEY_LATITUDE  = "latitude";
    inline static constexpr auto KV_KEY_LONGITUDE = "longitude";
    inline static constexpr auto KV_KEY_ALTITUDE  = "altitude";
    inline static constexpr auto KV_KEY_GEOID     = "geoid";
    inline static constexpr auto KV_KEY_PRESSURE  = "pressure";

    // Property keys for section "filter"
    inline static constexpr auto KV_KEY_MAX_DIST   = "maxDistance";
    inline static constexpr auto KV_KEY_MAX_HEIGHT = "maxHeight";

    // Property keys for feed sections
    inline static constexpr auto KV_KEY_HOST     = "host";
    inline static constexpr auto KV_KEY_PORT     = "port";
    inline static constexpr auto KV_KEY_PRIORITY = "priority";
    inline static constexpr auto KV_KEY_LOGIN    = "login";

    inline static constexpr auto PATH_FEEDS          = "general.feeds";
    inline static constexpr auto PATH_GND_MODE       = "general.gndMode";
    inline static constexpr auto PATH_SERVER_PORT    = "server.port";
    inline static constexpr auto PATH_SERVER_MAX_CON = "server.maxConnections";
    inline static constexpr auto PATH_LATITUDE       = "fallback.latitude";
    inline static constexpr auto PATH_LONGITUDE      = "fallback.longitude";
    inline static constexpr auto PATH_ALTITUDE       = "fallback.altitude";
    inline static constexpr auto PATH_GEOID          = "fallback.geoid";
    inline static constexpr auto PATH_PRESSURE       = "fallback.pressure";
    inline static constexpr auto PATH_MAX_DIST       = "filter.maxDistance";
    inline static constexpr auto PATH_MAX_HEIGHT     = "filter.maxHeight";

    bool                                       GroundMode;      ///< Ground mode state
    object::CGpsPosition const                 GpsPosition;     ///< Fallback position
    f64 const                                  AtmPressure;     ///< Atmospheric fallback pressure
    s32 const                                  MaxHeight;       ///< Maximum height for reported aircrafts
    s32 const                                  MaxDistance;     ///< Maximum distance for reported aircrafts
    std::tuple<u16, usize> const               ServerConfig;    ///< Port where to serve reports
    std::list<Str> const                       FeedNames;       ///< List of feed names
    std::unordered_map<Str, CProperties> const FeedProperties;  ///< Map feed names to their properties

    /**
     * @param stream The input stream
     * @throw std::logic_error if any error occurres
     */
    explicit CConfiguration(std::istream& stream_);
    ~CConfiguration() noexcept = default;
};

namespace error
{
class CConfigurationError : public vfrb::error::IError
{
public:
    CConfigurationError()                    = default;
    ~CConfigurationError() noexcept override = default;

    char const* What() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::config

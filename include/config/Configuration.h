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
/// Extractor for configuration file
class CConfiguration
{
    CProperties const m_properties;  ///< The properties from file

    /// @throw vfrb::config::error::CConversionError
    object::CGpsPosition resolvePosition() const;

    std::unordered_map<Str, CProperties> resolveFeeds() const;
    std::list<Str>                       resolveFeedNames() const;

    /// @param key_ The filters property name
    s32 resolveFilter(char const* key_) const;

    /// Dump the configuration to INFO log.
    void dumpInfo() const;

public:
    inline static constexpr auto SECT_KEY_FALLBACK = "fallback";
    inline static constexpr auto SECT_KEY_GENERAL  = "general";
    inline static constexpr auto SECT_KEY_FILTER   = "filter";
    inline static constexpr auto SECT_KEY_SERVER   = "server";

    inline static constexpr auto SECT_KEY_APRSC = "aprs";
    inline static constexpr auto SECT_KEY_SBS   = "sbs";
    inline static constexpr auto SECT_KEY_GPS   = "gps";
    inline static constexpr auto SECT_KEY_WIND  = "wind";
    inline static constexpr auto SECT_KEY_ATMOS = "atm";

    inline static constexpr auto KV_KEY_FEEDS    = "feeds";
    inline static constexpr auto KV_KEY_GND_MODE = "gndMode";

    inline static constexpr auto KV_KEY_MAX_CON = "maxConnections";

    inline static constexpr auto KV_KEY_LATITUDE  = "latitude";
    inline static constexpr auto KV_KEY_LONGITUDE = "longitude";
    inline static constexpr auto KV_KEY_ALTITUDE  = "altitude";
    inline static constexpr auto KV_KEY_GEOID     = "geoid";
    inline static constexpr auto KV_KEY_PRESSURE  = "pressure";

    inline static constexpr auto KV_KEY_MAX_DIST   = "maxDistance";
    inline static constexpr auto KV_KEY_MAX_HEIGHT = "maxHeight";

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

    bool                                       GroundMode;      ///< Ground mode enabled?
    object::CGpsPosition const                 GpsPosition;     ///< Fallback position
    f64 const                                  AtmPressure;     ///< Atmospheric fallback pressure
    s32 const                                  MaxHeight;       ///< Maximum height for reported aircrafts
    s32 const                                  MaxDistance;     ///< Maximum distance for reported aircrafts
    std::tuple<u16, usize> const               ServerConfig;    ///< Port where to serve reports
    std::list<Str> const                       FeedNames;       ///< List of feed names
    std::unordered_map<Str, CProperties> const FeedProperties;  ///< Map feed names to their properties

    /**
     * @param stream The input stream
     * @throw vfrb::config::error::CConfigurationError
     */
    explicit CConfiguration(std::istream& stream_);
    ~CConfiguration() noexcept = default;
};

namespace error
{
/// Error indicating that the configuration is not valid
class CConfigurationError : public vfrb::error::IError
{
public:
    CConfigurationError()                    = default;
    ~CConfigurationError() noexcept override = default;

    char const* Message() const noexcept override;
};

/// Error indicating that a string to number conversion failed
class CConversionError : public vfrb::error::IError
{
    Str const m_msg;

public:
    /**
     * @param str_  The value found at path
     * @param path_ The property path
     */
    CConversionError(Str const& str_, char const* path_);
    ~CConversionError() noexcept override = default;

    char const* Message() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::config

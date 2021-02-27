/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <istream>
#include <list>
#include <tuple>

#include "error/IError.hpp"
#include "object/CGpsPosition.hpp"

#include "CProperties.hpp"
#include "Types.hpp"

namespace vfrb::config
{
class CConfiguration
{
    CProperties const m_properties;

    auto
    resolvePosition() const -> object::CGpsPosition;

    auto
    resolveFeeds() const -> HashMap<String, CProperties>;
    auto
    resolveFeedNames() const -> List<String>;

    auto
    resolveFilter(str key_) const -> i32;

    void
    dumpInfo() const;

public:
    CTCONST SECT_KEY_FALLBACK = "fallback";
    CTCONST SECT_KEY_GENERAL  = "general";
    CTCONST SECT_KEY_FILTER   = "filter";
    CTCONST SECT_KEY_SERVER   = "server";

    CTCONST SECT_KEY_APRSC = "aprs";
    CTCONST SECT_KEY_SBS   = "sbs";
    CTCONST SECT_KEY_GPS   = "gps";
    CTCONST SECT_KEY_WIND  = "wind";
    CTCONST SECT_KEY_ATMOS = "atm";

    CTCONST KV_KEY_FEEDS    = "feeds";
    CTCONST KV_KEY_GND_MODE = "gndMode";

    CTCONST KV_KEY_MAX_CON = "maxConnections";

    CTCONST KV_KEY_LATITUDE  = "latitude";
    CTCONST KV_KEY_LONGITUDE = "longitude";
    CTCONST KV_KEY_ALTITUDE  = "altitude";
    CTCONST KV_KEY_GEOID     = "geoid";
    CTCONST KV_KEY_PRESSURE  = "pressure";

    CTCONST KV_KEY_MAX_DIST   = "maxDistance";
    CTCONST KV_KEY_MAX_HEIGHT = "maxHeight";

    CTCONST KV_KEY_HOST     = "host";
    CTCONST KV_KEY_PORT     = "port";
    CTCONST KV_KEY_PRIORITY = "priority";
    CTCONST KV_KEY_LOGIN    = "login";

    CTCONST PATH_FEEDS          = "general.feeds";
    CTCONST PATH_GND_MODE       = "general.gndMode";
    CTCONST PATH_SERVER_PORT    = "server.port";
    CTCONST PATH_SERVER_MAX_CON = "server.maxConnections";
    CTCONST PATH_LATITUDE       = "fallback.latitude";
    CTCONST PATH_LONGITUDE      = "fallback.longitude";
    CTCONST PATH_ALTITUDE       = "fallback.altitude";
    CTCONST PATH_GEOID          = "fallback.geoid";
    CTCONST PATH_PRESSURE       = "fallback.pressure";
    CTCONST PATH_MAX_DIST       = "filter.maxDistance";
    CTCONST PATH_MAX_HEIGHT     = "filter.maxHeight";

    bool                               GroundMode;
    object::CGpsPosition const         GpsPosition;
    f64 const                          AtmPressure;
    i32 const                          MaxHeight;
    i32 const                          MaxDistance;
    Tuple<u16, usize> const            ServerConfig;
    List<String> const                 FeedNames;
    HashMap<String, CProperties> const FeedProperties;

    explicit CConfiguration(std::istream& stream_);
};

namespace error
{
class CConfigurationError : public vfrb::error::IError
{
public:
    [[nodiscard]] auto
    Message() const noexcept -> str override;
};

class CConversionError : public vfrb::error::IError
{
    String const m_msg;

public:
    CConversionError(String const& str_, str path_);

    [[nodiscard]] auto
    Message() const noexcept -> str override;
};
}  // namespace error
}  // namespace vfrb::config

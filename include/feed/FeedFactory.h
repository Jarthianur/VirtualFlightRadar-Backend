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

#include "config/Properties.h"
#include "error/Error.hpp"
#include "util/class_utils.h"

#include "types.h"

namespace vfrb::config
{
class CConfiguration;
}  // namespace vfrb::config

namespace vfrb::data
{
class AircraftData;
class AtmosphereData;
class GpsData;
class WindData;
}  // namespace vfrb::data

namespace vfrb::feed
{
class Feed;

/**
 * @brief Factory for Feed creation.
 */
class FeedFactory
{
    SPtr<config::CConfiguration> m_config;        ///< Pointer to the Configuration
    SPtr<data::AircraftData>    m_aircraftData;  ///< Pointer to the AircraftData
    SPtr<data::AtmosphereData>  m_atmosData;     ///< Pointer to the AtmosphereData
    SPtr<data::GpsData>         m_gpsData;       ///< Pointer to the GpsData
    SPtr<data::WindData>        m_windData;      ///< Pointer to the WindData

    /**
     * @brief Make a new Feed.
     * @tparam T The Feed type
     * @note Must be fully specialized for every concrete Feed type T.
     * @param name  The feed name
     * @return a pointer to the concrete Feed
     * @throw std::logic_error from invoked constructors
     */
    template<typename T, ENABLE_IF(EXTENDS(T, Feed))>
    SPtr<T> makeFeed(Str const& name);

public:
    /**
     * @param config       The Configuration
     * @param aircraftData The AircraftData pointer
     * @param atmosData    The AtmosphereData pointer
     * @param gpsData      The GpsData pointer
     * @param windData     The WindData pointer
     */
    FeedFactory(SPtr<config::CConfiguration> config, SPtr<data::AircraftData> aircraftData,
                SPtr<data::AtmosphereData> atmosData, SPtr<data::GpsData> gpsData,
                SPtr<data::WindData> windData);
    ~FeedFactory() noexcept = default;

    /**
     * @brief Create a Feed.
     * @param name  The feed name
     * @return an optional unique pointer to the feed
     * @throw std::logic_error from invoked methods
     */
    SPtr<Feed> createFeed(Str const& name);
};

namespace error
{
class FeedCreationError : public vfrb::error::IError
{
    Str const m_msg;

public:
    FeedCreationError();
    ~FeedCreationError() noexcept override = default;

    char const* what() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::feed

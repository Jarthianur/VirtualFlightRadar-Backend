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

#include <memory>
#include <string>
#include <type_traits>

#include <boost/optional.hpp>

#include "config/Properties.h"

namespace config
{
class Configuration;
}  // namespace config

namespace data
{
class AircraftData;
class AtmosphereData;
class GpsData;
class WindData;
}  // namespace data

namespace feed
{
class Feed;

/**
 * @brief Factory for Feed creation.
 */
class FeedFactory
{
    std::shared_ptr<config::Configuration> m_config;        ///< Pointer to the Configuration
    std::shared_ptr<data::AircraftData>    m_aircraftData;  ///< Pointer to the AircraftData
    std::shared_ptr<data::AtmosphereData>  m_atmosData;     ///< Pointer to the AtmosphereData
    std::shared_ptr<data::GpsData>         m_gpsData;       ///< Pointer to the GpsData
    std::shared_ptr<data::WindData>        m_windData;      ///< Pointer to the WindData

    /**
     * @brief Make a new Feed.
     * @tparam T The Feed type
     * @note Must be fully specialized for every concrete Feed type T.
     * @param name  The feed name
     * @return a pointer to the concrete Feed
     * @throw std::logic_error from invoked constructors
     */
    template<typename T, typename std::enable_if<std::is_base_of<Feed, T>::value>::type* = nullptr>
    std::shared_ptr<T> makeFeed(const std::string& name);

public:
    /**
     * @param config       The Configuration
     * @param aircraftData The AircraftData pointer
     * @param atmosData    The AtmosphereData pointer
     * @param gpsData      The GpsData pointer
     * @param windData     The WindData pointer
     */
    FeedFactory(std::shared_ptr<config::Configuration> config,
                std::shared_ptr<data::AircraftData>    aircraftData,
                std::shared_ptr<data::AtmosphereData> atmosData, std::shared_ptr<data::GpsData> gpsData,
                std::shared_ptr<data::WindData> windData);
    ~FeedFactory() noexcept = default;

    /**
     * @brief Create a Feed.
     * @param name  The feed name
     * @return an optional unique pointer to the feed
     * @throw std::logic_error from invoked methods
     */
    boost::optional<std::shared_ptr<Feed>> createFeed(const std::string& name);
};
}  // namespace feed

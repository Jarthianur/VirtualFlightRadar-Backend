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
#include <boost/optional.hpp>

#include "../config/Configuration.h"

namespace data
{
class AircraftData;
class AtmosphereData;
class GpsData;
class WindData;
}  // namespace data

/// @namespace feed
namespace feed
{
class Feed;

/**
 * @class FeedFactory
 * @brief Factory for Feed creation.
 */
class FeedFactory
{
public:
    /**
     * @fn FeedFactory
     * @brief Constructor
     * @param crConfig      The Configuration
     * @param pAircraftData The AircraftData pointer
     * @param pAtmosData    The AtmosphereData pointer
     * @param pGpsData      The GpsData pointer
     * @param pWindData     The WindData pointer
     */
    FeedFactory(const config::Configuration& crConfig,
                std::shared_ptr<data::AircraftData> pAircraftData,
                std::shared_ptr<data::AtmosphereData> pAtmosData,
                std::shared_ptr<data::GpsData> pGpsData,
                std::shared_ptr<data::WindData> pWindData);

    /**
     * @fn ~FeedFactory
     * @brief Destructor
     */
    virtual ~FeedFactory() noexcept;

    /**
     * @fn createFeed
     * @brief Create a Feed.
     * @param crName  The feed name
     * @param crKvMap The properties
     * @return an optional unique pointer to the feed
     * @throw std::logic_error from invoked methods
     */
    boost::optional<std::unique_ptr<Feed>> createFeed(const std::string& crName,
                                                      const config::KeyValueMap& crKvMap);

private:
    /**
     * @fn makeFeed
     * @brief Make a new Feed.
     * @tparam T The Feed type
     * @note Must be fully specialized for every concrete Feed type T.
     * @param crName  The feed name
     * @param crKvMap The properties
     * @return a pointer to the concrete Feed
     * @throw std::logic_error from invoked constructors
     */
    template<typename T>
    T* makeFeed(const std::string& crName, const config::KeyValueMap& crKvMap);

    /// @var mrConfig
    /// Reference to the Configuration
    const config::Configuration& mrConfig;

    /// @var mpAircraftData
    /// Pointer to the AircraftData
    std::shared_ptr<data::AircraftData> mpAircraftData;

    /// @var mpAtmosphereData
    /// Pointer to the AtmosphereData
    std::shared_ptr<data::AtmosphereData> mpAtmosData;

    /// @var mpGpsData
    /// Pointer to the GpsData
    std::shared_ptr<data::GpsData> mpGpsData;

    /// @var mpWindData
    /// Pointer to the WindData
    std::shared_ptr<data::WindData> mpWindData;
};
}  // namespace feed

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
#include <memory>
#include <string>

#include "../Defines.h"
#include "../config/PropertyMap.h"
#include "parser/SensorParser.h"

#include "Feed.h"

/// @namespace data
namespace data
{
class AtmosphereData;
class WindData;
} /* namespace data */

/// @namespace feed
namespace feed
{
/**
 * @class SensorFeed
 * @brief Sensor input feed.
 * @extends Feed
 */
class SensorFeed : public Feed
{
public:
    NON_COPYABLE(SensorFeed)

    /**
     * @fn SensorFeed
     * @brief Constructor
     * @param crName     The SensorFeeds unique name
     * @param crKvMap    The properties map
     * @param pWindData  The WindData pointer
     * @param pAtmosData The AtmosphereData pointer
     * @throw std::logic_error from parent constructor
     */
    SensorFeed(const std::string& crName, const config::KeyValueMap& crKvMap,
               std::shared_ptr<data::WindData> pWindData,
               std::shared_ptr<data::AtmosphereData> pAtmosData);

    /**
     * @fn ~SensorFeed
     * @brief Destructor
     */
    virtual ~SensorFeed() noexcept;

    /**
     * @see Feed#process
     */
    void process(const std::string& crResponse) noexcept override;

private:
    /// @var mParser
    /// Parser to unpack response from Client
    parser::SensorParser mParser;

    /// @var mWindSlot
    /// WindData attempt slot
    std::size_t mWindSlot;

    /// @var mAtmosSlot
    /// AtmosphereData attempt slot
    std::size_t mAtmosSlot;

    /// @var mpWindData
    /// WindData to update for wind input
    std::shared_ptr<data::WindData> mpWindData;

    /// @var mpAtmosphereData
    /// AtmosphereData to update for atmospheric input
    std::shared_ptr<data::AtmosphereData> mpAtmosphereData;
};

}  // namespace feed

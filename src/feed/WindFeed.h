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

#include "../Defines.h"
#include "../config/PropertyMap.h"
#include "Feed.h"

namespace feed
{
namespace parser
{
class WindParser;
} /* namespace parser */
} /* namespace feed */

/// @namespace data
namespace data
{
class WindData;
}  // namespace data

/// @namespace feed
namespace feed
{
/**
 * @class WindFeed
 * @brief Sensor input feed.
 * @extends Feed
 */
class WindFeed : public Feed, public std::enable_shared_from_this<WindFeed>
{
public:
    NOT_COPYABLE(WindFeed)

    /**
     * @fn WindFeed
     * @brief Constructor
     * @param crName     The SensorFeeds unique name
     * @param crKvMap    The properties map
     * @param pWindData  The WindData pointer
     * @param pAtmosData The AtmosphereData pointer
     * @throw std::logic_error from parent constructor
     */
    WindFeed(const std::string& crName, const config::KeyValueMap& crKvMap,
             std::shared_ptr<data::WindData> pData);

    /**
     * @fn ~WindFeed
     * @brief Destructor
     */
    ~WindFeed() noexcept;

    void registerToClient(client::ClientManager& rManager) override;

    /**
     * @see Feed#process
     */
    void process(const std::string& crResponse) noexcept override;

private:
    /// @var mParser
    /// Parser to unpack response from Client
    static parser::WindParser smParser;
};

}  // namespace feed
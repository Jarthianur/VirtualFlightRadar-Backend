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
class AtmosphereParser;
} /* namespace parser */
} /* namespace feed */

/// @namespace data
namespace data
{
class AtmosphereData;
}  // namespace data

/// @namespace feed
namespace feed
{
/**
 * @class AtmosphereFeed
 * @brief Sensor input feed.
 * @extends Feed
 */
class AtmosphereFeed : public Feed
{
public:
    NOT_COPYABLE(AtmosphereFeed)

    /**
     * @fn AtmosphereFeed
     * @brief Constructor
     * @param crName     The SensorFeeds unique name
     * @param crKvMap    The properties map
     * @param pWindData  The WindData pointer
     * @param pAtmosData The AtmosphereData pointer
     * @throw std::logic_error from parent constructor
     */
    AtmosphereFeed(const std::string& crName, const config::KeyValueMap& crKvMap,
                   std::shared_ptr<data::AtmosphereData> pData);

    /**
     * @fn ~AtmosphereFeed
     * @brief Destructor
     */
    ~AtmosphereFeed() noexcept;

    Protocol get_protocol() const override;

    /**
     * @see Feed#process
     */
    bool process(const std::string& crResponse) noexcept override;

private:
    /// @var mParser
    /// Parser to unpack response from Client
    static parser::AtmosphereParser smParser;
};

}  // namespace feed

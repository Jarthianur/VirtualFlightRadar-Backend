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

#ifndef SRC_FEED_SENSORFEED_H_
#define SRC_FEED_SENSORFEED_H_

#include <cstdint>
#include <string>

#include "../config/PropertyMap.h"
#include "../parser/SensorParser.h"
#include "Feed.h"

namespace data
{
class WindData;
class AtmosphereData;
}

namespace feed
{
/**
 * @class SensorFeed extends Feed
 * @brief Represents a sensor input feed.
 * @see Feed.h
 */
class SensorFeed : public Feed
{
public:
    /**
     * @fn SensorFeed
     * @brief Constructor
     * @param cr_name  The SensorFeeds unique name
     * @param prio     The priority
     * @param cr_kvmap The properties map
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
     * @fn process
     * @brief Handle SensorClients response.
     * @param cr_res The response to process
     * @override Feed::process
     */
    void process(const std::string& crResponse) noexcept override;

private:
    /// Parser to unpack response from Client
    parser::SensorParser mParser;

    ///
    std::uint64_t mWindUpdateAttempts;

    ///
    std::uint64_t mAtmosUpdateAttempts;

    std::shared_ptr<data::WindData> mpWindData;

    std::shared_ptr<data::AtmosphereData> mpAtmosphereData;
};

}  // namespace feed

#endif /* SRC_FEED_SENSORFEED_H_ */

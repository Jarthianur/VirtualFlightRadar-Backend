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

#ifndef SRC_FEED_GPSFEED_H_
#define SRC_FEED_GPSFEED_H_

#include <cstdint>
#include <string>

#include "../config/PropertyMap.h"
#include "../parser/GpsParser.h"
#include "Feed.h"

namespace feed
{

/**
 * @class GpsFeed extends Feed
 * @brief Represents a GPS input feed.
 * @see Feed.h
 */
class GpsFeed: public Feed
{
public:
    /**
     * @fn GpsFeed
     * @brief Constructor
     * @param cr_name  The GpsFeeds unique name
     * @param prio     The priority
     * @param cr_kvmap The properties map
     */
    GpsFeed(const std::string& /*cr_name*/, std::uint32_t /*prio*/,
            const config::keyValueMap& /*cr_kvmap*/);
    /**
     * @fn ~GpsFeed
     * @brief Destructor
     */
    virtual ~GpsFeed() noexcept;
    /**
     * @fn process
     * @brief Handle GpsdClients response.
     * @param cr_res The response to process
     * @override Feed::process
     */
    void process(const std::string& cr_res) noexcept override;

private:
    /// Parser to unpack response from Client
    parser::GpsParser mParser;
    ///
    std::uint32_t mUpdateAttempts;
};

} // namespace feed

#endif /* SRC_FEED_GPSFEED_H_ */

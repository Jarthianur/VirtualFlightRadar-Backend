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
#include <cstdint>
#include <string>

#include "../config/PropertyMap.h"
#include "data/AircraftData.h"
#include "parser/AprsParser.h"
#include "Feed.h"

namespace feed
{
/**
 * @class AprscFeed extends Feed
 * @brief Represents an APRSC input feed.
 * @see Feed.h
 */
class AprscFeed : public Feed
{
public:
    /**
     * @fn AprscFeed
     * @brief Constructor
     * @param cr_name  The AprscFeeds unique name
     * @param prio     The priority
     * @param cr_kvmap The properties map
     * @throws std::logic_error if login is not given in cr_kvmap
     */
    AprscFeed(const std::string& /*cr_name*/, const config::KeyValueMap& /*cr_kvmap*/,
              std::shared_ptr<data::AircraftData> pData, std::int32_t vMaxHeight);
    /**
     * @fn ~AprscFeed
     * @brief Destructor
     */
    virtual ~AprscFeed() noexcept;
    /**
     * @fn process
     * @brief Handle AprscClients response.
     * @param cr_res The response to process
     * @override Feed::process
     */
    void process(const std::string& cr_res) noexcept override;

private:
    /// Parser to unpack response from Client
    parser::AprsParser mParser;

    std::size_t mDataSlot;

    std::shared_ptr<data::AircraftData> mpData;
};

}  // namespace feed

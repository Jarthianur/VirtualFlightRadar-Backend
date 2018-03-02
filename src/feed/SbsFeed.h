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

#include "../config/PropertyMap.h"
#include "../data/AircraftData.h"
#include "parser/SbsParser.h"
#include "Feed.h"

namespace feed
{
/**
 * @class SbsFeed extends Feed
 * @brief Represents a SBS input feed.
 * @see Feed.h
 */
class SbsFeed : public Feed
{
public:
    /**
     * @fn SbsFeed
     * @brief Constructor
     * @param cr_name  The SbsFeeds unique name
     * @param prio     The priority
     * @param cr_kvmap The properties map
     */
    SbsFeed(const std::string& cr_name, const config::KeyValueMap& cr_kvmap,
            std::shared_ptr<data::AircraftData> pData, std::int32_t vMaxHeight);
    /**
     * @fn ~SbsFeed
     * @brief Destructor
     */
    virtual ~SbsFeed() noexcept;
    /**
     * @fn process
     * @brief Handle SbsClients response.
     * @param cr_res The response to process
     * @override Feed::process
     */
    void process(const std::string& cr_res) noexcept override;

private:
    /// Parser to unpack response from Client
    parser::SbsParser mParser;

    std::size_t mDataSlot;

    std::shared_ptr<data::AircraftData> mpData;
};

}  // namespace feed

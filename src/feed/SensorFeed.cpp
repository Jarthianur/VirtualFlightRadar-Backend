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

#include "SensorFeed.h"

#include "../config/Configuration.h"
#include "../data/SensorData.h"
#include "../tcp/client/SensorClient.h"
#include "../util/Parser.h"
#include "../util/SensorInfo.h"
#include "../VFRB.h"

using namespace util;

namespace feed
{

SensorFeed::SensorFeed(const std::string& cr_name, std::int32_t prio,
        const std::unordered_map<std::string, std::string>& cr_kvmap)
        : Feed(cr_name, prio, cr_kvmap)
{
    mpClient = std::unique_ptr<tcp::client::Client>(
            new tcp::client::SensorClient(mKvMap.find(KV_KEY_HOST)->second,
                    mKvMap.find(KV_KEY_PORT)->second, *this));
}

SensorFeed::~SensorFeed() noexcept
{
}

std::int32_t SensorFeed::process(const std::string& cr_res) noexcept
{
    try
    {
        VFRB::msSensorData.update(Parser::parseSensNmea(cr_res), mPriority);
    } catch (const std::logic_error& e)
    {
        return -1;
    }
    return 0;
}

} // namespace feed
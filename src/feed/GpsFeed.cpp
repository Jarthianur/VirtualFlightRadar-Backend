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

#include "GpsFeed.h"

#include <memory>
#include <unordered_map>

#include "../config/Configuration.h"
#include "../data/GpsData.h"
#include "../tcp/client/GpsdClient.h"
#include "../util/Logger.h"
#include "../util/Position.hpp"
#include "../VFRB.h"

/// Define GPS metrics
#define GPS_NR_SATS_GOOD      7
#define GPS_FIX_GOOD          1
#define GPS_HOR_DILUTION_GOOD 1.0

using namespace util;

namespace feed
{

GpsFeed::GpsFeed(const std::string& cr_name, std::int32_t prio,
        const config::keyValueMap& cr_kvmap)
        : Feed(cr_name, prio, cr_kvmap)
{
    mpClient = std::unique_ptr<tcp::client::Client>(
            new tcp::client::GpsdClient(mKvMap.find(KV_KEY_HOST)->second,
                    mKvMap.find(KV_KEY_PORT)->second, *this));
}

GpsFeed::~GpsFeed() noexcept
{
}

void GpsFeed::process(const std::string& cr_res) noexcept
{
    struct ExtGpsPosition pos;
    if (mParser.unpack(cr_res, pos))
    {
        VFRB::msGpsData.update(pos, mPriority);
        if (config::Configuration::global_gnd_mode && pos.nrSats >= GPS_NR_SATS_GOOD
                && pos.fixQa >= GPS_FIX_GOOD && pos.dilution <= GPS_HOR_DILUTION_GOOD)
        {
            Logger::info("(GpsFeed) received good position -> stop");
            mpClient->stop();
        }
    }
}

} // namespace feed

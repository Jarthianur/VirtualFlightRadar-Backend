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
#include <boost/thread/lock_guard.hpp>

#include "../config/Configuration.h"
#include "../data/GpsData.h"
#include "../network/client/GpsdClient.h"
#include "../util/Logger.h"
#include "../util/Position.h"

/// Define GPS metrics
#define GPS_NR_SATS_GOOD 7
#define GPS_FIX_GOOD 1
#define GPS_HOR_DILUTION_GOOD 1.0

using namespace util;

namespace feed
{
GpsFeed::GpsFeed(const std::string& cr_name, const config::KeyValueMap& cr_kvmap,
                 data::GpsData* pData)
    : Feed(cr_name, cr_kvmap), mUpdateAttempts(0), mpData(pData)
{
    mpClient = std::unique_ptr<network::client::Client>(new network::client::GpsdClient(
        mKvMap.find(KV_KEY_HOST)->second, mKvMap.find(KV_KEY_PORT)->second, *this));
    mGndModeEnabled = !mKvMap.find(KV_KEY_GND_MODE)->second.empty();
    Logger::info("(GpsFeed) " +mName+ " - ground mode: ",
                 mGndModeEnabled ? "Yes" : "No");
}

GpsFeed::~GpsFeed() noexcept
{}

void GpsFeed::process(const std::string& cr_res) noexcept
{
    struct ExtGpsPosition pos;
    if(mParser.unpack(cr_res, pos))
    {
        boost::lock_guard<boost::mutex> lock(mpData->mMutex);
        try {
        mpData->update(pos, getPriority(), mUpdateAttempts);
        } catch(const std::runtime_error& e) {
            Logger::info("(GpsFeed) " + mName + ": ", e.what());
            mpClient->stop();
            return;
        }
        if(mGndModeEnabled && pos.nrSats >= GPS_NR_SATS_GOOD
           && pos.fixQa >= GPS_FIX_GOOD && pos.dilution <= GPS_HOR_DILUTION_GOOD)
        {
            Logger::info("(GpsFeed) " + mName, ": received good position -> stop");
            mpData->lockPosition();
            mpClient->stop();
        }
    }
}

}  // namespace feed

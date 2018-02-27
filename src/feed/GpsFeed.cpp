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
#include "../network/client/GpsdClient.h"
#include "../util/Logger.h"
#include "../util/Position.h"

using namespace util;

namespace feed
{
GpsFeed::GpsFeed(const std::string& cr_name, const config::KeyValueMap& cr_kvmap,
                 std::shared_ptr<data::GpsData> pData, bool vGndMode)
    : Feed(cr_name, cr_kvmap),
      mUpdateAttempts(0),
      mpData(pData),
      mGndModeEnabled(vGndMode)
{
    mpClient = std::unique_ptr<network::client::Client>(new network::client::GpsdClient(
        mKvMap.find(KV_KEY_HOST)->second, mKvMap.find(KV_KEY_PORT)->second, *this));
}

GpsFeed::~GpsFeed() noexcept
{}

void GpsFeed::process(const std::string& cr_res) noexcept
{
    struct ExtGpsPosition pos(getPriority(), mGndModeEnabled);
    if(mParser.unpack(cr_res, pos))
    {
        try
        {
            if(mpData->update(pos, getPriority(), mUpdateAttempts))
            {
                throw std::runtime_error("received good position -> stop");
            }
        }
        catch(const std::runtime_error& e)
        {
            Logger::info({"(GpsFeed) ", mName, ": ", e.what()});
            mpClient->stop();
            return;
        }
    }
}

}  // namespace feed

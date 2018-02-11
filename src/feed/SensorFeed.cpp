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

#include <memory>
#include <unordered_map>

#include "../config/Configuration.h"
#include "../data/AtmosphereData.h"
#include "../data/WindData.h"
#include "../network/client/SensorClient.h"
#include "../util/Sensor.h"
#include "../VFRB.h"

using namespace util;

namespace feed
{

SensorFeed::SensorFeed(const std::string& crName,
                       const config::KeyValueMap& crKvMap)
        : Feed(crName, crKvMap),
          mWindUpdateAttempts(0),
          mAtmosUpdateAttempts(0)

{
    mpClient = std::unique_ptr<network::client::Client>(
            new network::client::SensorClient(mKvMap.find(KV_KEY_HOST)->second,
                    mKvMap.find(KV_KEY_PORT)->second, *this));
}

SensorFeed::~SensorFeed() noexcept
{
}

void SensorFeed::process(const std::string& crResponse) noexcept
{
    struct Climate climate;
    if (mParser.unpack(crResponse, climate))
    {
        if (climate.hasWind())
        {
            VFRB::msWindData.update(climate.mWind, getPriority(), mWindUpdateAttempts);
        }
        if (climate.hasAtmosphere())
        {
            VFRB::msAtmosData.update(climate.mAtmosphere, getPriority(),
                    mAtmosUpdateAttempts);
        }
    }
}

} // namespace feed

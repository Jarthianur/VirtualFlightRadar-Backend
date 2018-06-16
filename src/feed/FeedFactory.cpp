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

#include "FeedFactory.h"

#include "../data/AircraftData.h"
#include "../data/AtmosphereData.h"
#include "../data/GpsData.h"
#include "../data/WindData.h"
#include "AprscFeed.h"
#include "GpsFeed.h"
#include "SbsFeed.h"
#include "AtmosphereFeed.h"

using namespace config;
using namespace data;

namespace feed
{
FeedFactory::FeedFactory(const Configuration& crConfig,
                         std::shared_ptr<AircraftData>& pAircraftData,
                         std::shared_ptr<AtmosphereData>& pAtmosData,
                         std::shared_ptr<GpsData>& pGpsData,
                         std::shared_ptr<WindData>& pWindData)
    : mrConfig(crConfig),
      mpAircraftData(pAircraftData),
      mpAtmosData(pAtmosData),
      mpGpsData(pGpsData),
      mpWindData(pWindData)
{}

FeedFactory::~FeedFactory() noexcept
{}

template<>
AprscFeed* FeedFactory::makeFeed<AprscFeed>(const std::string& crName,
                                            const KeyValueMap& crKvMap)
{
    return new AprscFeed(crName, crKvMap, mpAircraftData, mrConfig.getMaxHeight());
}

template<>
GpsFeed* FeedFactory::makeFeed<GpsFeed>(const std::string& crName,
                                        const KeyValueMap& crKvMap)
{
    return new GpsFeed(crName, crKvMap, mpGpsData);
}

template<>
SbsFeed* FeedFactory::makeFeed<SbsFeed>(const std::string& crName,
                                        const KeyValueMap& crKvMap)
{
    return new SbsFeed(crName, crKvMap, mpAircraftData, mrConfig.getMaxHeight());
}

template<>
SensorFeed* FeedFactory::makeFeed<SensorFeed>(const std::string& crName,
                                              const KeyValueMap& crKvMap)
{
    return new SensorFeed(crName, crKvMap, mpWindData, mpAtmosData);
}

boost::optional<std::shared_ptr<Feed>> FeedFactory::createFeed(const std::string& crName,
                                                               const KeyValueMap& crKvMap)
{
    if(crName.find(SECT_KEY_APRSC) != std::string::npos)
    {
        return std::shared_ptr<Feed>(makeFeed<AprscFeed>(crName, crKvMap));
    }
    else if(crName.find(SECT_KEY_SBS) != std::string::npos)
    {
        return std::shared_ptr<Feed>(makeFeed<SbsFeed>(crName, crKvMap));
    }
    else if(crName.find(SECT_KEY_GPS) != std::string::npos)
    {
        return std::shared_ptr<Feed>(makeFeed<GpsFeed>(crName, crKvMap));
    }
    else if(crName.find(SECT_KEY_SENS) != std::string::npos)
    {
        return std::shared_ptr<Feed>(makeFeed<SensorFeed>(crName, crKvMap));
    }
    return boost::none;
}

}  // namespace feed

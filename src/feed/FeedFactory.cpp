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

#include "../config/Configuration.h"
#include "../data/AircraftData.h"
#include "../data/AtmosphereData.h"
#include "../data/GpsData.h"
#include "../data/WindData.h"
#include "AprscFeed.h"
#include "AtmosphereFeed.h"
#include "GpsFeed.h"
#include "SbsFeed.h"
#include "WindFeed.h"

using namespace config;
using namespace data;

namespace feed
{
FeedFactory::FeedFactory(const Configuration& crConfig,
                         std::shared_ptr<AircraftData>& pAircraftData,
                         std::shared_ptr<AtmosphereData>& pAtmosData,
                         std::shared_ptr<GpsData>& pGpsData, std::shared_ptr<WindData>& pWindData)
    : mrConfig(crConfig),
      mpAircraftData(pAircraftData),
      mpAtmosData(pAtmosData),
      mpGpsData(pGpsData),
      mpWindData(pWindData)
{}

FeedFactory::~FeedFactory() noexcept
{}

template<>
std::shared_ptr<AprscFeed> FeedFactory::makeFeed<AprscFeed>(const std::string& crName,
                                                            const KeyValueMap& crKvMap)
{
    return std::make_shared<AprscFeed>(crName, crKvMap, mpAircraftData, mrConfig.getMaxHeight());
}

template<>
std::shared_ptr<GpsFeed> FeedFactory::makeFeed<GpsFeed>(const std::string& crName,
                                                        const KeyValueMap& crKvMap)
{
    return std::make_shared<GpsFeed>(crName, crKvMap, mpGpsData);
}

template<>
std::shared_ptr<SbsFeed> FeedFactory::makeFeed<SbsFeed>(const std::string& crName,
                                                        const KeyValueMap& crKvMap)
{
    return std::make_shared<SbsFeed>(crName, crKvMap, mpAircraftData, mrConfig.getMaxHeight());
}

template<>
std::shared_ptr<WindFeed> FeedFactory::makeFeed<WindFeed>(const std::string& crName,
                                                          const KeyValueMap& crKvMap)
{
    return std::make_shared<WindFeed>(crName, crKvMap, mpWindData);
}

template<>
std::shared_ptr<AtmosphereFeed> FeedFactory::makeFeed<AtmosphereFeed>(const std::string& crName,
                                                                      const KeyValueMap& crKvMap)
{
    return std::make_shared<AtmosphereFeed>(crName, crKvMap, mpAtmosData);
}

boost::optional<std::shared_ptr<Feed>> FeedFactory::createFeed(const std::string& crName,
                                                               const KeyValueMap& crKvMap)
{
    if(crName.find(SECT_KEY_APRSC) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(makeFeed<AprscFeed>(crName, crKvMap));
    }
    else if(crName.find(SECT_KEY_SBS) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(makeFeed<SbsFeed>(crName, crKvMap));
    }
    else if(crName.find(SECT_KEY_GPS) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(makeFeed<GpsFeed>(crName, crKvMap));
    }
    else if(crName.find(SECT_KEY_WIND) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(makeFeed<WindFeed>(crName, crKvMap));
    }
    else if(crName.find(SECT_KEY_ATMOS) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(
            makeFeed<AtmosphereFeed>(crName, crKvMap));
    }
    return boost::none;
}

}  // namespace feed

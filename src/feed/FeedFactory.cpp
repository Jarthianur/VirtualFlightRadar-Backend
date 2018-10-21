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
FeedFactory::FeedFactory(std::shared_ptr<config::Configuration> config,
                         std::shared_ptr<AircraftData>          aircraftData,
                         std::shared_ptr<AtmosphereData>        atmosData,
                         std::shared_ptr<GpsData> gpsData, std::shared_ptr<WindData> windData)
    : m_config(config),
      m_aircraftData(aircraftData),
      m_atmosData(atmosData),
      m_gpsData(gpsData),
      m_windData(windData)
{}

FeedFactory::~FeedFactory() noexcept {}

template<>
std::shared_ptr<AprscFeed> FeedFactory::makeFeed<AprscFeed>(const std::string& name,
                                                            const KeyValueMap& propertyMap)
{
    return std::make_shared<AprscFeed>(name, propertyMap, m_aircraftData,
                                       m_config->get_maxHeight());
}

template<>
std::shared_ptr<GpsFeed> FeedFactory::makeFeed<GpsFeed>(const std::string& name,
                                                        const KeyValueMap& propertyMap)
{
    return std::make_shared<GpsFeed>(name, propertyMap, m_gpsData);
}

template<>
std::shared_ptr<SbsFeed> FeedFactory::makeFeed<SbsFeed>(const std::string& name,
                                                        const KeyValueMap& propertyMap)
{
    return std::make_shared<SbsFeed>(name, propertyMap, m_aircraftData, m_config->get_maxHeight());
}

template<>
std::shared_ptr<WindFeed> FeedFactory::makeFeed<WindFeed>(const std::string& name,
                                                          const KeyValueMap& propertyMap)
{
    return std::make_shared<WindFeed>(name, propertyMap, m_windData);
}

template<>
std::shared_ptr<AtmosphereFeed>
    FeedFactory::makeFeed<AtmosphereFeed>(const std::string& name, const KeyValueMap& propertyMap)
{
    return std::make_shared<AtmosphereFeed>(name, propertyMap, m_atmosData);
}

boost::optional<std::shared_ptr<Feed>> FeedFactory::createFeed(const std::string& name,
                                                               const KeyValueMap& propertyMap)
{
    if (name.find(SECT_KEY_APRSC) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(makeFeed<AprscFeed>(name, propertyMap));
    }
    else if (name.find(SECT_KEY_SBS) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(makeFeed<SbsFeed>(name, propertyMap));
    }
    else if (name.find(SECT_KEY_GPS) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(makeFeed<GpsFeed>(name, propertyMap));
    }
    else if (name.find(SECT_KEY_WIND) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(makeFeed<WindFeed>(name, propertyMap));
    }
    else if (name.find(SECT_KEY_ATMOS) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(
            makeFeed<AtmosphereFeed>(name, propertyMap));
    }
    return boost::none;
}

}  // namespace feed

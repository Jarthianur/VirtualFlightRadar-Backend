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

#include "config/Configuration.h"
#include "data/AircraftData.h"
#include "data/AtmosphereData.h"
#include "data/GpsData.h"
#include "data/WindData.h"

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

template<>
std::shared_ptr<AprscFeed> FeedFactory::makeFeed<AprscFeed>(const std::string& name)
{
    return std::make_shared<AprscFeed>(name, m_config->get_feedProperties().at(name),
                                       m_aircraftData, m_config->get_maxHeight());
}

template<>
std::shared_ptr<GpsFeed> FeedFactory::makeFeed<GpsFeed>(const std::string& name)
{
    return std::make_shared<GpsFeed>(name, m_config->get_feedProperties().at(name), m_gpsData);
}

template<>
std::shared_ptr<SbsFeed> FeedFactory::makeFeed<SbsFeed>(const std::string& name)
{
    return std::make_shared<SbsFeed>(name, m_config->get_feedProperties().at(name), m_aircraftData,
                                     m_config->get_maxHeight());
}

template<>
std::shared_ptr<WindFeed> FeedFactory::makeFeed<WindFeed>(const std::string& name)
{
    return std::make_shared<WindFeed>(name, m_config->get_feedProperties().at(name), m_windData);
}

template<>
std::shared_ptr<AtmosphereFeed> FeedFactory::makeFeed<AtmosphereFeed>(const std::string& name)
{
    return std::make_shared<AtmosphereFeed>(name, m_config->get_feedProperties().at(name),
                                            m_atmosData);
}

boost::optional<std::shared_ptr<Feed>> FeedFactory::createFeed(const std::string& name)
{
    if (name.find(SECT_KEY_APRSC) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(makeFeed<AprscFeed>(name));
    }
    else if (name.find(SECT_KEY_SBS) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(makeFeed<SbsFeed>(name));
    }
    else if (name.find(SECT_KEY_GPS) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(makeFeed<GpsFeed>(name));
    }
    else if (name.find(SECT_KEY_WIND) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(makeFeed<WindFeed>(name));
    }
    else if (name.find(SECT_KEY_ATMOS) != std::string::npos)
    {
        return boost::make_optional<std::shared_ptr<Feed>>(makeFeed<AtmosphereFeed>(name));
    }
    return boost::none;
}

}  // namespace feed

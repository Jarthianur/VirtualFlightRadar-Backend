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

#include "feed/FeedFactory.h"

#include <stdexcept>
#include <string>

#include "config/Configuration.h"
#include "data/AircraftData.h"
#include "data/AtmosphereData.h"
#include "data/GpsData.h"
#include "data/WindData.h"
#include "feed/AprscFeed.h"
#include "feed/AtmosphereFeed.h"
#include "feed/GpsFeed.h"
#include "feed/SbsFeed.h"
#include "feed/WindFeed.h"

using namespace config;
using namespace data;

namespace feed
{
FeedFactory::FeedFactory(std::shared_ptr<config::Configuration> config,
                         std::shared_ptr<AircraftData>          aircraftData,
                         std::shared_ptr<AtmosphereData> atmosData, std::shared_ptr<GpsData> gpsData,
                         std::shared_ptr<WindData> windData)
    : m_config(config),
      m_aircraftData(aircraftData),
      m_atmosData(atmosData),
      m_gpsData(gpsData),
      m_windData(windData)
{}

template<>
std::shared_ptr<AprscFeed> FeedFactory::makeFeed<AprscFeed>(const std::string& name)
{
    return std::make_shared<AprscFeed>(name, m_config->feedProperties.at(name), m_aircraftData,
                                       m_config->maxHeight);
}

template<>
std::shared_ptr<GpsFeed> FeedFactory::makeFeed<GpsFeed>(const std::string& name)
{
    return std::make_shared<GpsFeed>(name, m_config->feedProperties.at(name), m_gpsData);
}

template<>
std::shared_ptr<SbsFeed> FeedFactory::makeFeed<SbsFeed>(const std::string& name)
{
    return std::make_shared<SbsFeed>(name, m_config->feedProperties.at(name), m_aircraftData,
                                     m_config->maxHeight);
}

template<>
std::shared_ptr<WindFeed> FeedFactory::makeFeed<WindFeed>(const std::string& name)
{
    return std::make_shared<WindFeed>(name, m_config->feedProperties.at(name), m_windData);
}

template<>
std::shared_ptr<AtmosphereFeed> FeedFactory::makeFeed<AtmosphereFeed>(const std::string& name)
{
    return std::make_shared<AtmosphereFeed>(name, m_config->feedProperties.at(name), m_atmosData);
}

std::shared_ptr<Feed> FeedFactory::createFeed(const std::string& name)
{
    if (name.find(Configuration::SECT_KEY_APRSC) != std::string::npos)
    {
        return makeFeed<AprscFeed>(name);
    }
    if (name.find(Configuration::SECT_KEY_SBS) != std::string::npos)
    {
        return makeFeed<SbsFeed>(name);
    }
    if (name.find(Configuration::SECT_KEY_GPS) != std::string::npos)
    {
        return makeFeed<GpsFeed>(name);
    }
    if (name.find(Configuration::SECT_KEY_WIND) != std::string::npos)
    {
        return makeFeed<WindFeed>(name);
    }
    if (name.find(Configuration::SECT_KEY_ATMOS) != std::string::npos)
    {
        return makeFeed<AtmosphereFeed>(name);
    }
    throw std::invalid_argument(std::string("no keywords found; be sure feed names contain one of ") +
                                Configuration::SECT_KEY_APRSC + ", " + Configuration::SECT_KEY_SBS + ", " +
                                Configuration::SECT_KEY_WIND + ", " + Configuration::SECT_KEY_ATMOS + ", " +
                                Configuration::SECT_KEY_GPS);
}

}  // namespace feed

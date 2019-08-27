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
using namespace std::literals;

namespace feed
{
FeedFactory::FeedFactory(s_ptr<config::Configuration> config, s_ptr<AircraftData> aircraftData,
                         s_ptr<AtmosphereData> atmosData, s_ptr<GpsData> gpsData, s_ptr<WindData> windData)
    : m_config(config),
      m_aircraftData(aircraftData),
      m_atmosData(atmosData),
      m_gpsData(gpsData),
      m_windData(windData)
{}

template<>
s_ptr<AprscFeed> FeedFactory::makeFeed<AprscFeed>(str const& name)
{
    return std::make_shared<AprscFeed>(name, m_config->feedProperties.at(name), m_aircraftData,
                                       m_config->maxHeight);
}

template<>
s_ptr<GpsFeed> FeedFactory::makeFeed<GpsFeed>(str const& name)
{
    return std::make_shared<GpsFeed>(name, m_config->feedProperties.at(name), m_gpsData);
}

template<>
s_ptr<SbsFeed> FeedFactory::makeFeed<SbsFeed>(str const& name)
{
    return std::make_shared<SbsFeed>(name, m_config->feedProperties.at(name), m_aircraftData,
                                     m_config->maxHeight);
}

template<>
s_ptr<WindFeed> FeedFactory::makeFeed<WindFeed>(str const& name)
{
    return std::make_shared<WindFeed>(name, m_config->feedProperties.at(name), m_windData);
}

template<>
s_ptr<AtmosphereFeed> FeedFactory::makeFeed<AtmosphereFeed>(str const& name)
{
    return std::make_shared<AtmosphereFeed>(name, m_config->feedProperties.at(name), m_atmosData);
}

s_ptr<Feed> FeedFactory::createFeed(str const& name)
{
    if (name.find(Configuration::SECT_KEY_APRSC) != str::npos)
    {
        return makeFeed<AprscFeed>(name);
    }
    if (name.find(Configuration::SECT_KEY_SBS) != str::npos)
    {
        return makeFeed<SbsFeed>(name);
    }
    if (name.find(Configuration::SECT_KEY_GPS) != str::npos)
    {
        return makeFeed<GpsFeed>(name);
    }
    if (name.find(Configuration::SECT_KEY_WIND) != str::npos)
    {
        return makeFeed<WindFeed>(name);
    }
    if (name.find(Configuration::SECT_KEY_ATMOS) != str::npos)
    {
        return makeFeed<AtmosphereFeed>(name);
    }
    throw std::invalid_argument("no keywords found; be sure feed names contain one of "s +
                                Configuration::SECT_KEY_APRSC + ", " + Configuration::SECT_KEY_SBS + ", " +
                                Configuration::SECT_KEY_WIND + ", " + Configuration::SECT_KEY_ATMOS + ", " +
                                Configuration::SECT_KEY_GPS);
}
}  // namespace feed

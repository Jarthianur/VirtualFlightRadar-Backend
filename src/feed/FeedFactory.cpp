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

using namespace vfrb::config;
using namespace vfrb::data;
using namespace std::literals;

namespace vfrb::feed
{
FeedFactory::FeedFactory(SPtr<config::CConfiguration> config, SPtr<AircraftData> aircraftData,
                         SPtr<AtmosphereData> atmosData, SPtr<GpsData> gpsData, SPtr<WindData> windData)
    : m_config(config),
      m_aircraftData(aircraftData),
      m_atmosData(atmosData),
      m_gpsData(gpsData),
      m_windData(windData)
{}

template<>
SPtr<AprscFeed> FeedFactory::makeFeed<AprscFeed>(Str const& name)
{
    return std::make_shared<AprscFeed>(name, m_config->feedProperties.at(name), m_aircraftData,
                                       m_config->maxHeight);
}

template<>
SPtr<GpsFeed> FeedFactory::makeFeed<GpsFeed>(Str const& name)
{
    return std::make_shared<GpsFeed>(name, m_config->feedProperties.at(name), m_gpsData);
}

template<>
SPtr<SbsFeed> FeedFactory::makeFeed<SbsFeed>(Str const& name)
{
    return std::make_shared<SbsFeed>(name, m_config->feedProperties.at(name), m_aircraftData,
                                     m_config->maxHeight);
}

template<>
SPtr<WindFeed> FeedFactory::makeFeed<WindFeed>(Str const& name)
{
    return std::make_shared<WindFeed>(name, m_config->feedProperties.at(name), m_windData);
}

template<>
SPtr<AtmosphereFeed> FeedFactory::makeFeed<AtmosphereFeed>(Str const& name)
{
    return std::make_shared<AtmosphereFeed>(name, m_config->feedProperties.at(name), m_atmosData);
}

SPtr<Feed> FeedFactory::createFeed(Str const& name)
{
    if (name.find(CConfiguration::SECT_KEY_APRSC) != Str::npos)
    {
        return makeFeed<AprscFeed>(name);
    }
    if (name.find(CConfiguration::SECT_KEY_SBS) != Str::npos)
    {
        return makeFeed<SbsFeed>(name);
    }
    if (name.find(CConfiguration::SECT_KEY_GPS) != Str::npos)
    {
        return makeFeed<GpsFeed>(name);
    }
    if (name.find(CConfiguration::SECT_KEY_WIND) != Str::npos)
    {
        return makeFeed<WindFeed>(name);
    }
    if (name.find(CConfiguration::SECT_KEY_ATMOS) != Str::npos)
    {
        return makeFeed<AtmosphereFeed>(name);
    }
    throw error::FeedCreationError();
}

namespace error
{
FeedCreationError::FeedCreationError()
    : m_msg("no keywords found; be sure feed names contain one of "s + Configuration::SECT_KEY_APRSC + ", " +
            Configuration::SECT_KEY_SBS + ", " + Configuration::SECT_KEY_WIND + ", " +
            Configuration::SECT_KEY_ATMOS + ", " + Configuration::SECT_KEY_GPS)
{}

char const* FeedCreationError::what() const noexcept
{
    return m_msg.c_str();
}
}  // namespace error
}  // namespace vfrb::feed

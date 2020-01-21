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
#include "util/string_utils.hpp"

using namespace vfrb::config;
using namespace vfrb::data;
using namespace vfrb::str_util;

namespace vfrb::feed
{
CFeedFactory::CFeedFactory(SPtr<config::CConfiguration> config_, SPtr<CAircraftData> aircraftData_,
                           SPtr<CAtmosphereData> atmosData_, SPtr<CGpsData> gpsData_,
                           SPtr<CWindData> windData_)
    : m_config(config_),
      m_aircraftData(aircraftData_),
      m_atmosData(atmosData_),
      m_gpsData(gpsData_),
      m_windData(windData_)
{}

template<>
SPtr<CAprscFeed> CFeedFactory::makeFeed<CAprscFeed>(Str const& name_)
{
    return std::make_shared<CAprscFeed>(name_, m_config->FeedProperties.at(name_), m_aircraftData,
                                        m_config->MaxHeight);
}

template<>
SPtr<CGpsFeed> CFeedFactory::makeFeed<CGpsFeed>(Str const& name_)
{
    return std::make_shared<CGpsFeed>(name_, m_config->FeedProperties.at(name_), m_gpsData);
}

template<>
SPtr<CSbsFeed> CFeedFactory::makeFeed<CSbsFeed>(Str const& name_)
{
    return std::make_shared<CSbsFeed>(name_, m_config->FeedProperties.at(name_), m_aircraftData,
                                      m_config->MaxHeight);
}

template<>
SPtr<CWindFeed> CFeedFactory::makeFeed<CWindFeed>(Str const& name_)
{
    return std::make_shared<CWindFeed>(name_, m_config->FeedProperties.at(name_), m_windData);
}

template<>
SPtr<CAtmosphereFeed> CFeedFactory::makeFeed<CAtmosphereFeed>(Str const& name_)
{
    return std::make_shared<CAtmosphereFeed>(name_, m_config->FeedProperties.at(name_), m_atmosData);
}

SPtr<IFeed> CFeedFactory::createFeed(Str const& name_)
{
    if (name_.find(CConfiguration::SECT_KEY_APRSC) != Str::npos)
    {
        return makeFeed<CAprscFeed>(name_);
    }
    if (name_.find(CConfiguration::SECT_KEY_SBS) != Str::npos)
    {
        return makeFeed<CSbsFeed>(name_);
    }
    if (name_.find(CConfiguration::SECT_KEY_GPS) != Str::npos)
    {
        return makeFeed<CGpsFeed>(name_);
    }
    if (name_.find(CConfiguration::SECT_KEY_WIND) != Str::npos)
    {
        return makeFeed<CWindFeed>(name_);
    }
    if (name_.find(CConfiguration::SECT_KEY_ATMOS) != Str::npos)
    {
        return makeFeed<CAtmosphereFeed>(name_);
    }
    throw error::CFeedCreationError();
}

namespace error
{
CFeedCreationError::CFeedCreationError()
    : m_msg(MakeStr("no keywords found; be sure feed names contain one of ", CConfiguration::SECT_KEY_APRSC,
                    ", ", CConfiguration::SECT_KEY_SBS, ", ", CConfiguration::SECT_KEY_WIND, ", ",
                    CConfiguration::SECT_KEY_ATMOS, ", ", CConfiguration::SECT_KEY_GPS))
{}

char const* CFeedCreationError::Message() const noexcept
{
    return m_msg.c_str();
}
}  // namespace error
}  // namespace vfrb::feed

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

#include "Configuration.h"

#include <iterator>
#include <stdexcept>
#include "../feed/AprscFeed.h"
#include "../feed/GpsFeed.h"
#include "../feed/SbsFeed.h"
#include "../feed/SensorFeed.h"
#include "../util/Logger.h"
#include "../util/Math.hpp"

#include "ConfigReader.h"

using namespace util;

namespace config
{
Configuration::Configuration(std::istream& r_stream)
{
    if(!init(r_stream))
    {
        throw std::logic_error("Failed to read configuration file");
    }
}

Configuration::~Configuration() noexcept
{}

/**
 * Static initializers
 */
std::int32_t Configuration::sBaseAltitude = 0;
double Configuration::sBaseLatitude       = 0.0;
double Configuration::sBaseLongitude      = 0.0;
double Configuration::sBaseGeoid          = 0.0;
double Configuration::sBaseAtmPressure    = 0.0;
std::int32_t Configuration::sMaxHeight    = 0;
std::int32_t Configuration::sMaxDistance  = 0;
std::uint16_t Configuration::sServerPort  = 1;
bool Configuration::sGndModeEnabled       = false;
std::vector<std::shared_ptr<feed::Feed>> Configuration::sRegisteredFeeds;

bool Configuration::init(std::istream& r_stream)
{
    ConfigReader reader;
    PropertyMap properties;

    try
    {
        reader.read(r_stream, properties);
    }
    catch(const std::exception& e)
    {
        Logger::error("(Config) read file: ", e.what());
        return false;
    }

    if(!resolveFallbacks(properties))
    {
        return false;
    }

    sMaxDistance = resolveFilter(properties, KV_KEY_MAX_DIST);
    sMaxHeight   = resolveFilter(properties, KV_KEY_MAX_HEIGHT);
    sServerPort  = resolveServerPort(
        properties.getProperty(SECT_KEY_GENERAL, KV_KEY_SERVER_PORT, "4353"));
    sGndModeEnabled = !properties.getProperty(SECT_KEY_GENERAL, KV_KEY_GND_MODE).empty();

    std::size_t nrf = registerFeeds(properties);
    Logger::info("(Config) number of feeds: ", std::to_string(nrf));

    return nrf > 0;
}

std::size_t Configuration::registerFeeds(const PropertyMap& cr_map)
{
    std::vector<std::string> feeds
        = resolveFeeds(cr_map.getProperty(SECT_KEY_GENERAL, KV_KEY_FEEDS));

    for(auto it = feeds.cbegin(); it != feeds.cend(); ++it)
    {
        // TODO put to feed construct
        std::uint64_t priority = 0;
        try
        {
            priority = std::stoul(cr_map.getProperty(*it, KV_KEY_PRIORITY, "0"));
            if(priority > UINT32_MAX)
            {
                throw std::invalid_argument("");
            }
        }
        catch(const std::logic_error&)
        {
            Logger::warn("(Config) create feed " + *it, ": Invalid priority given.");
            // Drop this feed, or create with priority 0?
            continue;
        }
        if(priority == 0)
        {
            Logger::warn("(Config) create feed " + *it,
                         ": Priority is 0; this feed cannot update higher ones.");
        }

        try
        {
            if(it->find(SECT_KEY_APRSC) != std::string::npos)
            {
                sRegisteredFeeds.push_back(
                    std::shared_ptr<feed::Feed>(new feed::AprscFeed(
                        *it, (std::uint32_t) priority, cr_map.getSectionKv(*it))));
            }
            else if(it->find(SECT_KEY_SBS) != std::string::npos)
            {
                sRegisteredFeeds.push_back(std::shared_ptr<feed::Feed>(new feed::SbsFeed(
                    *it, (std::uint32_t) priority, cr_map.getSectionKv(*it))));
            }
            else if(it->find(SECT_KEY_SENS) != std::string::npos)
            {
                sRegisteredFeeds.push_back(
                    std::shared_ptr<feed::Feed>(new feed::SensorFeed(
                        *it, (std::uint32_t) priority, cr_map.getSectionKv(*it))));
            }
            else if(it->find(SECT_KEY_GPS) != std::string::npos)
            {
                sRegisteredFeeds.push_back(std::shared_ptr<feed::Feed>(new feed::GpsFeed(
                    *it, (std::uint32_t) priority, cr_map.getSectionKv(*it))));
            }
            else
            {
                Logger::warn(
                    "(Config) create feed " + *it,
                    ": No keywords found; be sure feed names contain one of " SECT_KEY_APRSC
                    ", " SECT_KEY_SBS ", " SECT_KEY_SENS ", " SECT_KEY_GPS);
            }
        }
        catch(const std::exception& e)
        {
            Logger::warn("(Config) create feed " + *it + ": ", e.what());
        }
    }
    return sRegisteredFeeds.size();
}

template<>
double Configuration::resolveNumberKey<double>(const PropertyMap& cr_map,
                                               const std::string& crSection,
                                               const std::string& crKey,
                                               const std::string& crDefault)
{
    boost::tuple<bool, double> result = util::math::stringToNumber<double>(
        cr_map.getProperty(crSection, crKey, crDefault));
    if(!result.get<0>())
    {
        Logger::warn("(Config) " + crSection + "." + crKey, ": Could not resolve value.");
        throw std::invalid_argument("");
    }
    Logger::info("(Config) " + crSection + "." + crKey + ": ",
                 std::to_string(result.get<1>()));
    return result.get<1>();
}

template<>
std::int32_t Configuration::resolveNumberKey<std::int32_t>(const PropertyMap& cr_map,
                                                           const std::string& crSection,
                                                           const std::string& crKey,
                                                           const std::string& crDefault)
{
    boost::tuple<bool, std::int32_t> result = util::math::stringToNumber<std::int32_t>(
        cr_map.getProperty(crSection, crKey, crDefault));
    if(!result.get<0>())
    {
        Logger::warn("(Config) " + crSection + "." + crKey, ": Could not resolve value.");
        throw std::invalid_argument("");
    }
    Logger::info("(Config) " + crSection + "." + crKey + ": ",
                 std::to_string(result.get<1>()));
    return result.get<1>();
}

bool Configuration::resolveFallbacks(const PropertyMap& cr_map)
{
    try
    {
        sBaseLatitude
            = resolveNumberKey<double>(cr_map, SECT_KEY_FALLBACK, KV_KEY_LATITUDE, "");
        sBaseLongitude
            = resolveNumberKey<double>(cr_map, SECT_KEY_FALLBACK, KV_KEY_LONGITUDE, "");
        sBaseAltitude = resolveNumberKey<std::int32_t>(cr_map, SECT_KEY_FALLBACK,
                                                       KV_KEY_ALTITUDE, "");
        sBaseGeoid
            = resolveNumberKey<double>(cr_map, SECT_KEY_FALLBACK, KV_KEY_GEOID, "");
        sBaseAtmPressure = resolveNumberKey<double>(cr_map, SECT_KEY_FALLBACK,
                                                    KV_KEY_PRESSURE, "1013.25");
    }
    catch(const std::invalid_argument&)
    {
        return false;
    }
    return true;
}

std::int32_t Configuration::resolveFilter(const PropertyMap& cr_map,
                                          const std::string& crKey)
{
    try
    {
        std::int32_t tmp
            = resolveNumberKey<std::int32_t>(cr_map, SECT_KEY_FILTER, crKey, "-1");
        return tmp < 0 ? std::numeric_limits<std::int32_t>::max() : tmp;
    }
    catch(const std::invalid_argument&)
    {
        return std::numeric_limits<std::int32_t>::max();
    }
}

std::uint16_t Configuration::resolveServerPort(const std::string& cr_port)
{
    std::uint64_t port = 4353;
    try
    {
        port = std::stoul(cr_port);
        if(port > UINT16_MAX)
        {
            throw std::invalid_argument("");
        }
    }
    catch(const std::logic_error&)
    {
        port = 4353;
        Logger::warn("(Config) " KV_KEY_SERVER_PORT
                     ": Invalid server port; use default.");
    }
    Logger::info("(Config) " KV_KEY_SERVER_PORT ": ", std::to_string(sServerPort));
    return port & 0xFFFF;
}

std::vector<std::string> Configuration::resolveFeeds(const std::string& cr_feeds)
{
    std::vector<std::string> feeds;
    std::stringstream ss;
    ss.str(cr_feeds);
    std::string item;

    while(std::getline(ss, item, ','))
    {
        std::size_t f = item.find_first_not_of(' ');
        if(f != std::string::npos)
        {
            item = item.substr(f);
        }
        std::size_t l = item.find_last_not_of(' ');
        if(l != std::string::npos)
        {
            item = item.substr(0, l + 1);
        }
        feeds.push_back(item);
    }
    return feeds;
}

}  // namespace config

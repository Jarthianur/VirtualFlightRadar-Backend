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
#include <sstream>
#include <stdexcept>

#include "../feed/AprscFeed.h"
#include "../feed/GpsFeed.h"
#include "../feed/SbsFeed.h"
#include "../feed/SensorFeed.h"
#include "../util/Logger.h"
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

    resolveFallbacks(properties);
    resolveFilters(properties);
    resolveServerPort(properties);

    sGndModeEnabled = !properties.getProperty(SECT_KEY_GENERAL, KV_KEY_GND_MODE).empty();

    std::size_t nrf = registerFeeds(properties);
    Logger::info("(Config) number of feeds: ", std::to_string(nrf));

    return nrf > 0;
}

std::size_t Configuration::registerFeeds(const PropertyMap& cr_map)
{
    std::vector<std::string> feeds;
    std::stringstream ss;
    ss.str(cr_map.getProperty(SECT_KEY_GENERAL, KV_KEY_FEEDS));
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

    for(auto it = feeds.cbegin(); it != feeds.cend(); ++it)
    {
        std::uint64_t priority = 0;
        try
        {
            priority = std::stoul(cr_map.getProperty(*it, KV_KEY_PRIORITY, "0"));
            if(priority > UINT32_MAX)
            {
                throw std::invalid_argument("");
            }
        }
        catch(const std::logic_error& e)
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

void Configuration::resolveFallbacks(const PropertyMap& cr_map)
{
    // resolve latitude
    try
    {
        sBaseLatitude
            = std::stod(cr_map.getProperty(SECT_KEY_FALLBACK, KV_KEY_LATITUDE, "0.0"));
        Logger::info("(Config) " KV_KEY_LATITUDE ": ", std::to_string(sBaseLatitude));
    }
    catch(const std::logic_error& e)
    {
        Logger::warn("(Config) " KV_KEY_LATITUDE ": Can not resolve fallback.");
    }

    // resolve longitude
    try
    {
        sBaseLongitude
            = std::stod(cr_map.getProperty(SECT_KEY_FALLBACK, KV_KEY_LONGITUDE, "0.0"));
        Logger::info("(Config) " KV_KEY_LONGITUDE ": ", std::to_string(sBaseLongitude));
    }
    catch(const std::logic_error& e)
    {
        Logger::warn("(Config) " KV_KEY_LONGITUDE ": Can not resolve fallback.");
    }

    // resolve altitude
    try
    {
        sBaseAltitude
            = std::stoi(cr_map.getProperty(SECT_KEY_FALLBACK, KV_KEY_ALTITUDE, "0"));
        Logger::info("(Config) " KV_KEY_ALTITUDE ": ", std::to_string(sBaseAltitude));
    }
    catch(const std::logic_error& e)
    {
        Logger::warn("(Config) " KV_KEY_ALTITUDE ": Can not resolve fallback.");
    }

    // resolve geoid
    try
    {
        sBaseGeoid
            = std::stod(cr_map.getProperty(SECT_KEY_FALLBACK, KV_KEY_GEOID, "0.0"));
        Logger::info("(Config) " KV_KEY_GEOID ": ", std::to_string(sBaseGeoid));
    }
    catch(const std::logic_error& e)
    {
        Logger::warn("(Config) " KV_KEY_GEOID ": Can not resolve fallback.");
    }

    // resolve atmospheric pressure, ICAO standard as default
    try
    {
        sBaseAtmPressure = std::stod(
            cr_map.getProperty(SECT_KEY_FALLBACK, KV_KEY_PRESSURE, "1013.25"));
        Logger::info("(Config) " KV_KEY_PRESSURE ": ", std::to_string(sBaseAtmPressure));
    }
    catch(const std::logic_error& e)
    {
        Logger::warn("(Config) " KV_KEY_PRESSURE ": Can not resolve fallback.");
    }
}

void Configuration::resolveFilters(const PropertyMap& cr_map)
{
    std::string tmp = cr_map.getProperty(SECT_KEY_FILTER, KV_KEY_MAX_HEIGHT, "-1");
    if(tmp == "-1")
    {
        sMaxHeight = INT32_MAX;
    }
    else
    {
        try
        {
            sMaxHeight = std::stoi(tmp);
        }
        catch(const std::logic_error& e)
        {
            sMaxHeight = INT32_MAX;
            Logger::warn("(Config) " KV_KEY_MAX_HEIGHT
                         ": Can not resolve filter; thus disabled.");
        }
    }
    Logger::info("(Config) " KV_KEY_MAX_HEIGHT ": ", std::to_string(sMaxHeight));

    tmp = cr_map.getProperty(SECT_KEY_FILTER, KV_KEY_MAX_DIST, "-1");
    if(tmp == "-1")
    {
        sMaxDistance = INT32_MAX;
    }
    else
    {
        try
        {
            sMaxDistance = std::stoi(tmp);
        }
        catch(const std::logic_error& e)
        {
            sMaxDistance = INT32_MAX;
            Logger::warn("(Config) " KV_KEY_MAX_DIST
                         ": Can not resolve filter; thus disabled.");
        }
    }
    Logger::info("(Config) " KV_KEY_MAX_DIST ": ", std::to_string(sMaxDistance));
}

void Configuration::resolveServerPort(const PropertyMap& cr_map)
{
    std::uint64_t port = 4353;
    try
    {
        port = std::stoul(
            cr_map.getProperty(SECT_KEY_GENERAL, KV_KEY_SERVER_PORT, "4353"));
        if(port > UINT16_MAX)
        {
            throw std::invalid_argument("");
        }
    }
    catch(const std::logic_error& e)
    {
        port = 4353;
        Logger::warn("(Config) " KV_KEY_SERVER_PORT
                     ": Invalid server port; use default.");
    }
    global_server_port = (std::uint16_t) port;
    Logger::info("(Config) " KV_KEY_SERVER_PORT ": ", std::to_string(global_server_port));
}

}  // namespace config

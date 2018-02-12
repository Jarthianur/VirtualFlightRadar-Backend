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
Configuration::Configuration(std::istream& rStream)
{
    if(!init(rStream))
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

bool Configuration::init(std::istream& rStream)
{
    ConfigReader reader;
    PropertyMap properties;

    try
    {
        reader.read(rStream, properties);
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

    sMaxDistance    = resolveFilter(properties, KV_KEY_MAX_DIST);
    sMaxHeight      = resolveFilter(properties, KV_KEY_MAX_HEIGHT);
    sServerPort     = resolveServerPort(properties);
    sGndModeEnabled = !properties.getProperty(SECT_KEY_GENERAL, KV_KEY_GND_MODE).empty();
    std::size_t nrf = registerFeeds(properties);

    dumpInfo();
    Logger::info("(Config) number of feeds: ", std::to_string(nrf));
    return nrf > 0;
}

std::size_t Configuration::registerFeeds(const PropertyMap& crProperties)
{
    std::vector<std::string> feeds
        = resolveFeeds(crProperties.getProperty(SECT_KEY_GENERAL, KV_KEY_FEEDS));

    for(auto it = feeds.cbegin(); it != feeds.cend(); ++it)
    {
        try
        {
            if(it->find(SECT_KEY_APRSC) != std::string::npos)
            {
                sRegisteredFeeds.push_back(std::shared_ptr<feed::Feed>(
                    new feed::AprscFeed(*it, crProperties.getSectionKv(*it))));
            }
            else if(it->find(SECT_KEY_SBS) != std::string::npos)
            {
                sRegisteredFeeds.push_back(std::shared_ptr<feed::Feed>(
                    new feed::SbsFeed(*it, crProperties.getSectionKv(*it))));
            }
            else if(it->find(SECT_KEY_SENS) != std::string::npos)
            {
                sRegisteredFeeds.push_back(std::shared_ptr<feed::Feed>(
                    new feed::SensorFeed(*it, crProperties.getSectionKv(*it))));
            }
            else if(it->find(SECT_KEY_GPS) != std::string::npos)
            {
                sRegisteredFeeds.push_back(std::shared_ptr<feed::Feed>(
                    new feed::GpsFeed(*it, crProperties.getSectionKv(*it))));
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

math::Number Configuration::resolveNumberValue(
    const boost::tuple<bool, util::math::Number>& crOptNumber,
    const std::string& crSection, const std::string& crKey)
{
    if(!crOptNumber.get<0>())
    {
        Logger::warn("(Config) " + crSection + "." + crKey, ": Could not resolve value.");
        throw std::invalid_argument("");
    }
    return crOptNumber.get<1>();
}

void Configuration::dumpInfo() const
{
    Logger::info("(Config) " SECT_KEY_FALLBACK "." KV_KEY_LATITUDE ": ",
                 std::to_string(sBaseLatitude));
    Logger::info("(Config) " SECT_KEY_FALLBACK "." KV_KEY_LONGITUDE ": ",
                 std::to_string(sBaseLongitude));
    Logger::info("(Config) " SECT_KEY_FALLBACK "." KV_KEY_ALTITUDE ": ",
                 std::to_string(sBaseAltitude));
    Logger::info("(Config) " SECT_KEY_FALLBACK "." KV_KEY_GEOID ": ",
                 std::to_string(sBaseGeoid));
    Logger::info("(Config) " SECT_KEY_FALLBACK "." KV_KEY_PRESSURE ": ",
                 std::to_string(sBaseAtmPressure));
    Logger::info("(Config) " SECT_KEY_FILTER "." KV_KEY_MAX_HEIGHT ": ",
                 std::to_string(sMaxHeight));
    Logger::info("(Config) " SECT_KEY_FILTER "." KV_KEY_MAX_DIST ": ",
                 std::to_string(sMaxDistance));
    Logger::info("(Config) " SECT_KEY_GENERAL "." KV_KEY_SERVER_PORT ": ",
                 std::to_string(sServerPort));
    Logger::info("(Config) " SECT_KEY_GENERAL "." KV_KEY_GND_MODE ": ",
                 sGndModeEnabled ? "Yes" : "No");
}

bool Configuration::resolveFallbacks(const PropertyMap& crProperties)
{
    try
    {
        sBaseLatitude
            = resolveNumberValue(math::stringToNumber<double>(crProperties.getProperty(
                                     SECT_KEY_FALLBACK, KV_KEY_LATITUDE, "")),
                                 SECT_KEY_FALLBACK, KV_KEY_LATITUDE)
                  .float64;
        sBaseLongitude
            = resolveNumberValue(math::stringToNumber<double>(crProperties.getProperty(
                                     SECT_KEY_FALLBACK, KV_KEY_LONGITUDE, "")),
                                 SECT_KEY_FALLBACK, KV_KEY_LONGITUDE)
                  .float64;
        sBaseAltitude = resolveNumberValue(
                            math::stringToNumber<std::int32_t>(crProperties.getProperty(
                                SECT_KEY_FALLBACK, KV_KEY_ALTITUDE, "")),
                            SECT_KEY_FALLBACK, KV_KEY_ALTITUDE)
                            .int32;
        sBaseGeoid
            = resolveNumberValue(math::stringToNumber<double>(crProperties.getProperty(
                                     SECT_KEY_FALLBACK, KV_KEY_GEOID, "")),
                                 SECT_KEY_FALLBACK, KV_KEY_GEOID)
                  .float64;
        sBaseAtmPressure
            = resolveNumberValue(math::stringToNumber<double>(crProperties.getProperty(
                                     SECT_KEY_FALLBACK, KV_KEY_PRESSURE, "1013.25")),
                                 SECT_KEY_FALLBACK, KV_KEY_PRESSURE)
                  .float64;
    }
    catch(const std::invalid_argument&)
    {
        return false;
    }
    return true;
}

std::int32_t Configuration::resolveFilter(const PropertyMap& crProperties,
                                          const std::string& crKey)
{
    try
    {
        std::int32_t tmp
            = resolveNumberValue(
                  math::stringToNumber<std::int32_t>(
                      crProperties.getProperty(SECT_KEY_FILTER, crKey, "-1")),
                  SECT_KEY_FILTER, crKey)
                  .int32;
        return tmp < 0 ? std::numeric_limits<std::int32_t>::max() : tmp;
    }
    catch(const std::invalid_argument&)
    {
        return std::numeric_limits<std::int32_t>::max();
    }
}

std::uint16_t Configuration::resolveServerPort(const PropertyMap& crProperties)
{
    try
    {
        std::uint64_t port
            = resolveNumberValue(
                  math::stringToNumber<std::uint64_t>(crProperties.getProperty(
                      SECT_KEY_GENERAL, KV_KEY_SERVER_PORT, "4353")),
                  SECT_KEY_GENERAL, KV_KEY_SERVER_PORT)
                  .uint64;
        if(port > std::numeric_limits<std::uint16_t>::max())
        {
            throw std::invalid_argument("");
        }
        return port & 0xFFFF;
    }
    catch(const std::logic_error&)
    {
        return 4353;
    }
}

std::vector<std::string> Configuration::resolveFeeds(const std::string& crFeeds)
{
    std::vector<std::string> feeds;
    std::stringstream ss;
    ss.str(crFeeds);
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

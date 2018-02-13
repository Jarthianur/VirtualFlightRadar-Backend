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

    if(!setFallbacks(properties))
    {
        return false;
    }

    sMaxDistance    = resolveFilter(properties, KV_KEY_MAX_DIST);
    sMaxHeight      = resolveFilter(properties, KV_KEY_MAX_HEIGHT);
    sServerPort     = resolveServerPort(properties);
    sGndModeEnabled = !properties.getProperty(SECT_KEY_GENERAL, KV_KEY_GND_MODE).empty();

    dumpInfo();
    return true;
}

bool Configuration::setFallbacks(const PropertyMap& crProperties)
{
    try
    {
        sBaseLatitude = boost::get<double>(
            checkNumberValue(math::stringToNumber<double>(crProperties.getProperty(
                                 SECT_KEY_FALLBACK, KV_KEY_LATITUDE, "")),
                             SECT_KEY_FALLBACK, KV_KEY_LATITUDE));
        sBaseLongitude = boost::get<double>(
            checkNumberValue(math::stringToNumber<double>(crProperties.getProperty(
                                 SECT_KEY_FALLBACK, KV_KEY_LONGITUDE, "")),
                             SECT_KEY_FALLBACK, KV_KEY_LONGITUDE));
        sBaseAltitude = boost::get<std::int32_t>(
            checkNumberValue(math::stringToNumber<std::int32_t>(crProperties.getProperty(
                                 SECT_KEY_FALLBACK, KV_KEY_ALTITUDE, "")),
                             SECT_KEY_FALLBACK, KV_KEY_ALTITUDE));
        sBaseGeoid = boost::get<double>(
            checkNumberValue(math::stringToNumber<double>(crProperties.getProperty(
                                 SECT_KEY_FALLBACK, KV_KEY_GEOID, "")),
                             SECT_KEY_FALLBACK, KV_KEY_GEOID));
        sBaseAtmPressure = boost::get<double>(
            checkNumberValue(math::stringToNumber<double>(crProperties.getProperty(
                                 SECT_KEY_FALLBACK, KV_KEY_PRESSURE, "1013.25")),
                             SECT_KEY_FALLBACK, KV_KEY_PRESSURE));
    }
    // Don't catch boost::bad_get (runtime) as it only occurres on implementation fault
    // (actually compiletime).
    catch(const std::invalid_argument&)
    {
        return false;
    }
    return true;
}

std::uint16_t Configuration::resolveServerPort(const PropertyMap& crProperties) const
{
    try
    {
        std::uint64_t port = boost::get<std::uint64_t>(
            checkNumberValue(math::stringToNumber<std::uint64_t>(crProperties.getProperty(
                                 SECT_KEY_GENERAL, KV_KEY_SERVER_PORT, "4353")),
                             SECT_KEY_GENERAL, KV_KEY_SERVER_PORT));
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

std::vector<std::pair<std::string, KeyValueMap>> Configuration::getFeeds() const
{}

std::vector<std::string> Configuration::resolveFeeds(const std::string& crFeeds) const
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

std::int32_t Configuration::resolveFilter(const PropertyMap& crProperties,
                                          const std::string& crKey) const
{
    try
    {
        std::int32_t tmp = boost::get<std::int32_t>(
            checkNumberValue(math::stringToNumber<std::int32_t>(
                                 crProperties.getProperty(SECT_KEY_FILTER, crKey, "-1")),
                             SECT_KEY_FILTER, crKey));
        return tmp < 0 ? std::numeric_limits<std::int32_t>::max() : tmp;
    }
    catch(const std::invalid_argument&)
    {
        return std::numeric_limits<std::int32_t>::max();
    }
}

math::Number Configuration::checkNumberValue(const math::OptNumber& crOptNumber,
                                             const std::string& crSection,
                                             const std::string& crKey) const
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
    Logger::info("(Config) number of feeds: ", std::to_string(sRegisteredFeeds.size()));
}

bool Configuration::getSGndModeEnabled() const
{
    return sGndModeEnabled;
}

std::uint16_t Configuration::getSServerPort() const
{
    return sServerPort;
}

std::int32_t Configuration::getSMaxDistance() const
{
    return sMaxDistance;
}

std::int32_t Configuration::getSMaxHeight() const
{
    return sMaxHeight;
}

double Configuration::getSBaseAtmPressure() const
{
    return sBaseAtmPressure;
}

double Configuration::getSBaseGeoid() const
{
    return sBaseGeoid;
}

std::int32_t Configuration::getSBaseAltitude() const
{
    return sBaseAltitude;
}

double Configuration::getSBaseLongitude() const
{
    return sBaseLongitude;
}

double Configuration::getSBaseLatitude() const
{
    return sBaseLatitude;
}

}  // namespace config

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

#include <limits>
#include <sstream>
#include <stdexcept>
#include <boost/tuple/tuple.hpp>
#include <boost/variant.hpp>

#include "../Logger.h"
#include "ConfigReader.h"

namespace config
{
using namespace util;

Configuration::Configuration(std::istream& rStream)
{
    try
    {
        ConfigReader reader;
        PropertyMap properties;
        reader.read(rStream, properties);
        init(properties);
    }
    catch(const std::exception&)
    {
        throw std::runtime_error("Failed to read configuration file");
    }
}

Configuration::~Configuration() noexcept
{}

void Configuration::init(const PropertyMap& crProperties)
{
    mAtmPressure = boost::get<double>(
        checkNumberValue(stringToNumber<double>(crProperties.getProperty(
                             SECT_KEY_FALLBACK, KV_KEY_PRESSURE, "1013.25")),
                         SECT_KEY_FALLBACK, KV_KEY_PRESSURE));
    mPosition    = resolvePosition(crProperties);
    mMaxDistance = resolveFilter(crProperties, KV_KEY_MAX_DIST);
    mMaxHeight   = resolveFilter(crProperties, KV_KEY_MAX_HEIGHT);
    mServerPort  = resolveServerPort(crProperties);
    mFeedMapping = resolveFeeds(crProperties);

    dumpInfo();
}

data::object::ExtGpsPosition
Configuration::resolvePosition(const PropertyMap& crProperties) const
{
    data::object::GpsPosition pos;
    pos.latitude = boost::get<double>(
        checkNumberValue(stringToNumber<double>(crProperties.getProperty(
                             SECT_KEY_FALLBACK, KV_KEY_LATITUDE, "")),
                         SECT_KEY_FALLBACK, KV_KEY_LATITUDE));
    pos.longitude = boost::get<double>(
        checkNumberValue(stringToNumber<double>(crProperties.getProperty(
                             SECT_KEY_FALLBACK, KV_KEY_LONGITUDE, "")),
                         SECT_KEY_FALLBACK, KV_KEY_LONGITUDE));
    pos.altitude = boost::get<std::int32_t>(
        checkNumberValue(stringToNumber<std::int32_t>(crProperties.getProperty(
                             SECT_KEY_FALLBACK, KV_KEY_ALTITUDE, "")),
                         SECT_KEY_FALLBACK, KV_KEY_ALTITUDE));
    double geoid = boost::get<double>(
        checkNumberValue(stringToNumber<double>(crProperties.getProperty(
                             SECT_KEY_FALLBACK, KV_KEY_GEOID, "")),
                         SECT_KEY_FALLBACK, KV_KEY_GEOID));
    return data::object::ExtGpsPosition(
        pos, geoid, !crProperties.getProperty(SECT_KEY_GENERAL, KV_KEY_GND_MODE).empty());
}

std::uint16_t Configuration::resolveServerPort(const PropertyMap& crProperties) const
{
    try
    {
        std::uint64_t port = boost::get<std::uint64_t>(
            checkNumberValue(stringToNumber<std::uint64_t>(crProperties.getProperty(
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

std::int32_t Configuration::resolveFilter(const PropertyMap& crProperties,
                                          const std::string& crKey) const
{
    try
    {
        std::int32_t tmp = boost::get<std::int32_t>(
            checkNumberValue(stringToNumber<std::int32_t>(
                                 crProperties.getProperty(SECT_KEY_FILTER, crKey, "-1")),
                             SECT_KEY_FILTER, crKey));
        return tmp < 0 ? std::numeric_limits<std::int32_t>::max() : tmp;
    }
    catch(const std::invalid_argument&)
    {
        return std::numeric_limits<std::int32_t>::max();
    }
}

FeedMapping Configuration::resolveFeeds(const PropertyMap& crProperties)
{
    std::list<std::string> list
        = splitCommaSeparated(crProperties.getProperty(SECT_KEY_GENERAL, KV_KEY_FEEDS));
    FeedMapping mapping;
    for(auto& it : list)
    {
        mapping.push_back(std::make_pair(it, crProperties.getSectionKeyValue(it)));
    }
    return mapping;
}

Number Configuration::checkNumberValue(const OptNumber& crOptNumber,
                                       const std::string& crSection,
                                       const std::string& crKey) const
{
    if(!crOptNumber.get<0>())
    {
        Logger::warn({"(Config) ", crSection, ".", crKey, ": Could not resolve value."});
        throw std::invalid_argument("");
    }
    return crOptNumber.get<1>();
}

void Configuration::dumpInfo() const
{
    Logger::info({"(Config) " SECT_KEY_FALLBACK "." KV_KEY_LATITUDE ": ",
                  std::to_string(mPosition.position.latitude)});
    Logger::info({"(Config) " SECT_KEY_FALLBACK "." KV_KEY_LONGITUDE ": ",
                  std::to_string(mPosition.position.longitude)});
    Logger::info({"(Config) " SECT_KEY_FALLBACK "." KV_KEY_ALTITUDE ": ",
                  std::to_string(mPosition.position.altitude)});
    Logger::info({"(Config) " SECT_KEY_FALLBACK "." KV_KEY_GEOID ": ",
                  std::to_string(mPosition.geoid)});
    Logger::info({"(Config) " SECT_KEY_FALLBACK "." KV_KEY_PRESSURE ": ",
                  std::to_string(mAtmPressure)});
    Logger::info({"(Config) " SECT_KEY_FILTER "." KV_KEY_MAX_HEIGHT ": ",
                  std::to_string(mMaxHeight)});
    Logger::info({"(Config) " SECT_KEY_FILTER "." KV_KEY_MAX_DIST ": ",
                  std::to_string(mMaxDistance)});
    Logger::info({"(Config) " SECT_KEY_GENERAL "." KV_KEY_SERVER_PORT ": ",
                  std::to_string(mServerPort)});
    Logger::info({"(Config) " SECT_KEY_GENERAL "." KV_KEY_GND_MODE ": ",
                  mPosition.ground ? "Yes" : "No"});
    Logger::info({"(Config) number of feeds: ", std::to_string(mFeedMapping.size())});
}

std::uint16_t Configuration::getServerPort() const
{
    return mServerPort;
}

std::int32_t Configuration::getMaxDistance() const
{
    return mMaxDistance;
}

std::int32_t Configuration::getMaxHeight() const
{
    return mMaxHeight;
}

double Configuration::getAtmPressure() const
{
    return mAtmPressure;
}

data::object::ExtGpsPosition Configuration::getPosition() const
{
    return mPosition;
}

bool Configuration::isGndModeEnabled() const
{
    return mPosition.ground;
}

void Configuration::forceGndMode()
{
    mPosition.ground = true;
}

const FeedMapping& Configuration::getFeedMapping() const
{
    return mFeedMapping;
}

}  // namespace config

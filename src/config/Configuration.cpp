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
#include <stdexcept>
#include <utility>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include "../Logger.hpp"
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
    m_atmPressure
        = boost::get<double>(checkNumberValue(stringToNumber<double>(crProperties.getProperty(
                                                  SECT_KEY_FALLBACK, KV_KEY_PRESSURE, "1013.25")),
                                              SECT_KEY_FALLBACK, KV_KEY_PRESSURE));
    m_position    = resolvePosition(crProperties);
    m_maxDistance = resolveFilter(crProperties, KV_KEY_MAX_DIST);
    m_maxHeight   = resolveFilter(crProperties, KV_KEY_MAX_HEIGHT);
    m_serverPort  = resolveServerPort(crProperties);
    m_groundMode  = !crProperties.getProperty(SECT_KEY_GENERAL, KV_KEY_GND_MODE).empty();
    m_feedMapping = resolveFeeds(crProperties);

    dumpInfo();
}

object::GpsPosition Configuration::resolvePosition(const PropertyMap& crProperties) const
{
    object::Position pos;
    pos.latitude = boost::get<double>(checkNumberValue(
        stringToNumber<double>(crProperties.getProperty(SECT_KEY_FALLBACK, KV_KEY_LATITUDE, "0.0")),
        SECT_KEY_FALLBACK, KV_KEY_LATITUDE));
    pos.longitude
        = boost::get<double>(checkNumberValue(stringToNumber<double>(crProperties.getProperty(
                                                  SECT_KEY_FALLBACK, KV_KEY_LONGITUDE, "0.0")),
                                              SECT_KEY_FALLBACK, KV_KEY_LONGITUDE));
    pos.altitude = boost::get<std::int32_t>(
        checkNumberValue(stringToNumber<std::int32_t>(
                             crProperties.getProperty(SECT_KEY_FALLBACK, KV_KEY_ALTITUDE, "0")),
                         SECT_KEY_FALLBACK, KV_KEY_ALTITUDE));
    double geoid = boost::get<double>(checkNumberValue(
        stringToNumber<double>(crProperties.getProperty(SECT_KEY_FALLBACK, KV_KEY_GEOID, "0.0")),
        SECT_KEY_FALLBACK, KV_KEY_GEOID));
    return object::GpsPosition(pos, geoid);
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
        std::int32_t filter = boost::get<std::int32_t>(checkNumberValue(
            stringToNumber<std::int32_t>(crProperties.getProperty(SECT_KEY_FILTER, crKey, "-1")),
            SECT_KEY_FILTER, crKey));
        return filter < 0 ? std::numeric_limits<std::int32_t>::max() : filter;
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
        try
        {
            mapping.push_back(std::make_pair(it, crProperties.getSectionKeyValue(it)));
        }
        catch(const std::out_of_range& e)
        {
            logger.warn("(Config) resolveFeeds: ", e.what(), " for ", it);
        }
    }
    return mapping;
}

Number Configuration::checkNumberValue(const OptNumber& crOptNumber, const std::string& crSection,
                                       const std::string& crKey) const
{
    if(!crOptNumber)
    {
        logger.warn("(Config) ", crSection, ".", crKey, ": Could not resolve value.");
        throw std::invalid_argument("");
    }
    return *crOptNumber;
}

void Configuration::dumpInfo() const
{
    logger.info("(Config) " SECT_KEY_FALLBACK "." KV_KEY_LATITUDE ": ",
                std::to_string(m_position.get_position().latitude));
    logger.info("(Config) " SECT_KEY_FALLBACK "." KV_KEY_LONGITUDE ": ",
                std::to_string(m_position.get_position().longitude));
    logger.info("(Config) " SECT_KEY_FALLBACK "." KV_KEY_ALTITUDE ": ",
                std::to_string(m_position.get_position().altitude));
    logger.info("(Config) " SECT_KEY_FALLBACK "." KV_KEY_GEOID ": ",
                std::to_string(m_position.get_geoid()));
    logger.info("(Config) " SECT_KEY_FALLBACK "." KV_KEY_PRESSURE ": ",
                std::to_string(m_atmPressure));
    logger.info("(Config) " SECT_KEY_FILTER "." KV_KEY_MAX_HEIGHT ": ",
                std::to_string(m_maxHeight));
    logger.info("(Config) " SECT_KEY_FILTER "." KV_KEY_MAX_DIST ": ",
                std::to_string(m_maxDistance));
    logger.info("(Config) " SECT_KEY_GENERAL "." KV_KEY_SERVER_PORT ": ",
                std::to_string(m_serverPort));
    logger.info("(Config) " SECT_KEY_GENERAL "." KV_KEY_GND_MODE ": ", m_groundMode ? "Yes" : "No");
    logger.info("(Config) number of feeds: ", std::to_string(m_feedMapping.size()));
}

}  // namespace config

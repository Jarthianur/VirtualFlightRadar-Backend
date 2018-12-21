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

#include "config/Configuration.h"

#include <limits>
#include <stdexcept>
#include <utility>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include "config/ConfigReader.h"
#include "util/Logger.hpp"

using namespace util;

namespace config
{
Configuration::Configuration(std::istream& stream)
{
    try
    {
        Properties properties = ConfigReader(stream).read();
        m_atmPressure         = boost::get<double>(
            checkNumber(stringToNumber<double>(properties.get_property(PATH_PRESSURE, "1013.25")),
                        PATH_PRESSURE));
        m_position    = resolvePosition(properties);
        m_maxDistance = resolveFilter(properties, KV_KEY_MAX_DIST);
        m_maxHeight   = resolveFilter(properties, KV_KEY_MAX_HEIGHT);
        m_serverPort  = resolveServerPort(properties);
        m_groundMode  = !properties.get_property(PATH_GND_MODE).empty();
        resolveFeeds(properties);
        dumpInfo();
    }
    catch (const std::exception& e)
    {
        logger.error("(Config) init: ", e.what());
        throw std::logic_error("Failed to read configuration file");
    }
}

object::GpsPosition Configuration::resolvePosition(const Properties& properties) const
{
    object::Position pos;
    pos.latitude  = boost::get<double>(checkNumber(
        stringToNumber<double>(properties.get_property(PATH_LATITUDE, "0.0")), PATH_LATITUDE));
    pos.longitude = boost::get<double>(checkNumber(
        stringToNumber<double>(properties.get_property(PATH_LONGITUDE, "0.0")), PATH_LONGITUDE));
    pos.altitude  = boost::get<std::int32_t>(checkNumber(
        stringToNumber<std::int32_t>(properties.get_property(PATH_ALTITUDE, "0")), PATH_ALTITUDE));
    double geoid  = boost::get<double>(checkNumber(
        stringToNumber<double>(properties.get_property(PATH_GEOID, "0.0")), PATH_GEOID));
    return object::GpsPosition(pos, geoid);
}

std::uint16_t Configuration::resolveServerPort(const Properties& properties) const
{
    try
    {
        std::uint64_t port = boost::get<std::uint64_t>(checkNumber(
            stringToNumber<std::uint64_t>(properties.get_property(PATH_SERVER_PORT, "4353")),
            PATH_SERVER_PORT));
        if (port > std::numeric_limits<std::uint16_t>::max())
        {
            throw std::invalid_argument("");
        }
        return port & 0xFFFF;
    }
    catch (const std::logic_error&)
    {
        return 4353;
    }
}

std::int32_t Configuration::resolveFilter(const Properties&  properties,
                                          const std::string& key) const
{
    try
    {
        std::string  path   = std::string(SECT_KEY_FILTER ".") + key;
        std::int32_t filter = boost::get<std::int32_t>(
            checkNumber(stringToNumber<std::int32_t>(properties.get_property(path, "-1")), path));
        return filter < 0 ? std::numeric_limits<std::int32_t>::max() : filter;
    }
    catch (const std::invalid_argument&)
    {
        return std::numeric_limits<std::int32_t>::max();
    }
}

void Configuration::resolveFeeds(const Properties& properties)
{
    for (auto& it : splitCommaSeparated(properties.get_property(PATH_FEEDS)))
    {
        try
        {
            m_feedProperties.emplace(it, properties.get_propertySection(it));
            m_feedNames.push_back(it);
        }
        catch (const std::out_of_range& e)
        {
            logger.warn("(Config) resolveFeeds: ", e.what(), " for ", it);
        }
    }
}

Number Configuration::checkNumber(const OptNumber& number, const std::string& path) const
{
    if (!number)
    {
        logger.warn("(Config) ", path, ": Could not resolve value.");
        throw std::invalid_argument("");
    }
    return *number;
}

void Configuration::dumpInfo() const
{
    logger.info("(Config) ", PATH_LATITUDE, ": ", m_position.get_position().latitude);
    logger.info("(Config) ", PATH_LONGITUDE, ": ", m_position.get_position().longitude);
    logger.info("(Config) ", PATH_ALTITUDE, ": ", m_position.get_position().altitude);
    logger.info("(Config) ", PATH_GEOID, ": ", m_position.get_geoid());
    logger.info("(Config) ", PATH_PRESSURE, ": ", m_atmPressure);
    logger.info("(Config) ", PATH_MAX_HEIGHT, ": ", m_maxHeight);
    logger.info("(Config) ", PATH_MAX_DIST, ": ", m_maxDistance);
    logger.info("(Config) ", PATH_SERVER_PORT, ": ", m_serverPort);
    logger.info("(Config) ", PATH_GND_MODE, ": ", m_groundMode ? "Yes" : "No");
    logger.info("(Config) number of feeds: ", m_feedProperties.size());
}
}  // namespace config

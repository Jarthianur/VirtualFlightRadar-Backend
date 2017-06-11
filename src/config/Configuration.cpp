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

#include <sys/types.h>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>

#include "../util/Logger.h"
using namespace util;

namespace config
{

Configuration::Configuration(const char* file)
{
    if (!init(file))
    {
        throw std::logic_error("failed to read configuration file");
    }
}

Configuration::~Configuration() noexcept
{
}

std::int32_t Configuration::base_altitude = 0;
double Configuration::base_latitude = 0.0;
double Configuration::base_longitude = 0.0;
double Configuration::base_geoid = 0.0;
double Configuration::base_pressure = 0.0;
std::int32_t Configuration::filter_maxHeight = 0;
std::int32_t Configuration::filter_maxDist = 0;
std::uint16_t Configuration::global_server_port = 1;
bool Configuration::global_gnd_mode = false;

std::vector<vfrb::Feed> Configuration::global_feeds;

bool Configuration::init(const char* file)
{
    ConfigReader cr(file);
    try
    {
        cr.read();
    } catch (const std::exception& e)
    {
        Logger::error("(Config) read file: ", e.what());
        return false;
    } catch (...)
    {
        Logger::error("(Config) read file");
        return false;
    }
    // get fallbacks
    base_latitude = strToDouble(
            cr.getProperty(SECT_KEY_FALLBACK, KV_KEY_LATITUDE, "0.0"));
    Logger::info("(Config) " KV_KEY_LATITUDE ": ",
            std::to_string(base_latitude));
    base_longitude = strToDouble(
            cr.getProperty(SECT_KEY_FALLBACK, KV_KEY_LONGITUDE, "0.0"));
    Logger::info("(Config) " KV_KEY_LONGITUDE ": ",
            std::to_string(base_longitude));
    base_altitude = strToInt(
            cr.getProperty(SECT_KEY_FALLBACK, KV_KEY_ALTITUDE, "0"));
    Logger::info("(Config) " KV_KEY_ALTITUDE ": ",
            std::to_string(base_altitude));
    base_geoid = strToDouble(
            cr.getProperty(SECT_KEY_FALLBACK, KV_KEY_GEOID, "0.0"));
    Logger::info("(Config) " KV_KEY_GEOID ": ", std::to_string(base_geoid));
    base_pressure = strToDouble(
            cr.getProperty(SECT_KEY_FALLBACK, KV_KEY_PRESSURE, "1013.25"));
    Logger::info("(Config) " KV_KEY_PRESSURE ": ",
            std::to_string(base_pressure));

    // get filters
    std::string tmp = cr.getProperty(SECT_KEY_FILTER, KV_KEY_MAX_HEIGHT, "-1");
    if (tmp == "-1")
    {
        filter_maxHeight = INT32_MAX;
    } else
    {
        filter_maxHeight = strToInt(tmp);
    }
    Logger::info("(Config) " KV_KEY_MAX_HEIGHT ": ",
            std::to_string(filter_maxHeight));

    tmp = cr.getProperty(SECT_KEY_FILTER, KV_KEY_MAX_DIST, "-1");
    if (tmp == "-1")
    {
        filter_maxDist = INT32_MAX;
    } else
    {
        filter_maxDist = strToInt(tmp);
    }
    Logger::info("(Config) " KV_KEY_MAX_DIST ": ",
            std::to_string(filter_maxDist));

    // get general
    global_gnd_mode = cr.getProperty(SECT_KEY_GENERAL, KV_KEY_GND_MODE) != "";

    global_server_port = (uint16_t) strToInt(
            cr.getProperty(SECT_KEY_GENERAL, KV_KEY_SERVER_PORT, "9999"));
    Logger::info("(Config) " KV_KEY_SERVER_PORT ": ",
            std::to_string(global_server_port));

    std::size_t nrf = registerFeeds(cr);
    Logger::info("(Config) number of feeds: ", std::to_string(nrf));

    return nrf > 0;
}

std::size_t Configuration::registerFeeds(ConfigReader& r_cr)
{
    std::vector<std::string> feeds;
    std::stringstream ss;
    ss.str(r_cr.getProperty(SECT_KEY_GENERAL, KV_KEY_FEEDS));
    std::string item;
    while (std::getline(ss, item, ','))
    {
        std::size_t f = item.find_first_not_of(' ');
        if (f != std::string::npos)
        {
            item = item.substr(f);
        }
        std::size_t l = item.find_last_not_of(' ');
        if (l != std::string::npos)
        {
            item = item.substr(0, l + 1);
        }
        feeds.push_back(item);
    }

    for (auto it = feeds.cbegin(); it != feeds.cend(); ++it)
    {
        if (it->find(SECT_KEY_APRSC) != std::string::npos)
        {
            try
            {
                vfrb::Feed f(*it,
                        strToInt(r_cr.getProperty(*it, KV_KEY_PRIORITY, "0")),
                        vfrb::Feed::InputType::APRSC, r_cr.getSectionKV(*it));
                global_feeds.push_back(std::move(f));
            } catch (const std::out_of_range& e)
            {
                Logger::warn("(Config) create feed " + *it + ": ", e.what());
            }
        } else if (it->find(SECT_KEY_SBS) != std::string::npos)
        {
            try
            {
                vfrb::Feed f(*it,
                        strToInt(r_cr.getProperty(*it, KV_KEY_PRIORITY, "0")),
                        vfrb::Feed::InputType::SBS, r_cr.getSectionKV(*it));
                global_feeds.push_back(std::move(f));
            } catch (const std::out_of_range& e)
            {
                Logger::warn("(Config) create feed " + *it + ": ", e.what());
            }
        } else if (it->find(SECT_KEY_SENS) != std::string::npos)
        {
            try
            {
                vfrb::Feed f(*it,
                        strToInt(r_cr.getProperty(*it, KV_KEY_PRIORITY, "0")),
                        vfrb::Feed::InputType::SENSOR, r_cr.getSectionKV(*it));
                global_feeds.push_back(std::move(f));
            } catch (const std::out_of_range& e)
            {
                Logger::warn("(Config) create feed " + *it + ": ", e.what());
            }
        } else if (it->find(SECT_KEY_GPS) != std::string::npos)
        {
            try
            {
                vfrb::Feed f(*it,
                        strToInt(r_cr.getProperty(*it, KV_KEY_PRIORITY, "0")),
                        vfrb::Feed::InputType::GPS, r_cr.getSectionKV(*it));
                global_feeds.push_back(std::move(f));
            } catch (const std::out_of_range& e)
            {
                Logger::warn("(Config) create feed " + *it + ": ", e.what());
            }
        }
    }
    return global_feeds.size();
}

std::int32_t Configuration::strToInt(const std::string& cr_str) noexcept
{
    try
    {
        return std::stoi(cr_str);
    } catch (const std::logic_error& iae)
    {
        Logger::warn("(VFRB) invalid configuration: ",
                cr_str.length() == 0 ? "empty" : cr_str);
    }
    return 0;
}

double Configuration::strToDouble(const std::string& cr_str) noexcept
{
    try
    {
        return std::stod(cr_str);
    } catch (const std::logic_error& iae)
    {
        Logger::warn("(VFRB) invalid configuration: ",
                cr_str.length() == 0 ? "empty" : cr_str);
    }
    return 0.0;
}

}  // namespace config

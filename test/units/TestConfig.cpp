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

#include <iterator>
#include <sstream>
#include <stdexcept>

#include "../../src/config/ConfigReader.h"
#include "../../src/config/Configuration.h"
#include "../../src/config/PropertyMap.h"
#include "../Helper.hpp"
#include "../framework/src/framework.h"

#ifdef assert
#undef assert
#endif

using namespace config;
using namespace testsuite;
using namespace comparator;

void test_config(TestSuitesRunner& runner)
{
    describe<ConfigReader>("read config", runner)->test("read", []() {
        std::stringstream conf;
        conf << "[" << SECT_KEY_FALLBACK << "]\n" << KV_KEY_LATITUDE << "   = 0.000000\n";
        conf << KV_KEY_LONGITUDE << " = \n" << KV_KEY_ALTITUDE << "=1000; alt\n;ghsgd";
        ConfigReader cr;
        PropertyMap map;
        cr.read(conf, map);
        assert(map.getProperty(SECT_KEY_FALLBACK, KV_KEY_LATITUDE, "invalid"),
               std::string("0.000000"), helper::equalsStr);
        assert(map.getProperty(SECT_KEY_FALLBACK, KV_KEY_LONGITUDE, "invalid"),
               std::string("invalid"), helper::equalsStr);
        assert(map.getProperty(SECT_KEY_FALLBACK, KV_KEY_ALTITUDE, "invalid"),
               std::string("1000"), helper::equalsStr);
        assert(map.getProperty(SECT_KEY_FALLBACK, KV_KEY_GEOID, "invalid"),
               std::string("invalid"), helper::equalsStr);
        assert(map.getProperty("nothing", ""), std::string(""), helper::equalsStr);
    });

    describe<Configuration>("initialize configuration", runner)
        ->test("valid config - full, one feed",
               []() {
                   std::stringstream conf_in;
                   conf_in << "[" << SECT_KEY_GENERAL << "]\n"
                           << KV_KEY_FEEDS << "=sens1\n";
                   conf_in << KV_KEY_SERVER_PORT << "=1234\n"
                           << KV_KEY_GND_MODE << "=y\n";
                   conf_in << "[" << SECT_KEY_FALLBACK << "]\n"
                           << KV_KEY_LATITUDE << "=77.777777\n";
                   conf_in << KV_KEY_LONGITUDE << "=-12.121212\n"
                           << KV_KEY_ALTITUDE << "=1234\n";
                   conf_in << KV_KEY_GEOID << "=40.4\n" << KV_KEY_PRESSURE << "=999.9\n";
                   conf_in << "[" << SECT_KEY_FILTER << "]\n"
                           << KV_KEY_MAX_HEIGHT << "=-1\n";
                   conf_in << KV_KEY_MAX_DIST << "=10000\n";
                   conf_in << "[sens1]\n" << KV_KEY_HOST << "=localhost\n";
                   conf_in << KV_KEY_PORT << "=3456\n" << KV_KEY_PRIORITY << "=1\n";
                   Configuration config(conf_in);
                   const auto feed_it = config.getFeedMapping().cbegin();
                   assert(feed_it->first, std::string("sens1"), helper::equalsStr);
                   assert(feed_it->second.at(KV_KEY_PRIORITY), std::string("1"),
                          helper::equalsStr);
                   assert(static_cast<std::int32_t>(config.getServerPort()), 1234,
                          helper::equalsInt);
                   assert(config.getGroundMode(), true, helper::equalsBool);
                   assert(config.getPosition().getPosition().latitude, 77.777777,
                          helper::equalsD);
                   assert(config.getPosition().getPosition().longitude, -12.121212,
                          helper::equalsD);
                   assert(config.getPosition().getPosition().altitude, 1234,
                          helper::equalsInt);
                   assert(config.getPosition().getGeoid(), 40.4, helper::equalsD);
                   assert(config.getAtmPressure(), 999.9, helper::equalsD);
                   assert(config.getMaxHeight(), INT32_MAX, helper::equalsInt);
                   assert(config.getMaxDistance(), 10000, helper::equalsInt);
               })
        ->test("only valid feeds", []() {
            std::stringstream conf_in;
            conf_in << "[" << SECT_KEY_GENERAL << "]\n"
                    << KV_KEY_FEEDS << "=sens,sbs1 , sbs2, else,,\n";
            conf_in << "[sens]\n"
                    << KV_KEY_HOST << "=127.0.0.1\n"
                    << KV_KEY_PORT << "=3333\n"
                    << KV_KEY_PRIORITY << "=0\n";
            conf_in << "[sbs1]\n"
                    << KV_KEY_HOST << "=127.0.0.1\n"
                    << KV_KEY_PORT << "=3334\n"
                    << KV_KEY_PRIORITY << "=1\n";
            Configuration config(conf_in);
            std::string valid("sens,sbs1,");
            std::string result;
            for(const auto& it : config.getFeedMapping())
            {
                result += it.first + ",";
            }
            assert(result, valid, helper::equalsStr);
        });
}

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

#include "../src/config/ConfigReader.h"
#include "../src/config/Configuration.h"
#include "../src/config/Properties.h"

#include "helper.hpp"

using namespace config;
using namespace sctf;

void test_config(test::TestSuitesRunner& runner)
{
    describe<ConfigReader>("read config", runner)->test("read", [] {
        std::stringstream conf_in;
        conf_in << "[" SECT_KEY_FALLBACK "]\n" << KV_KEY_LATITUDE "   = 0.000000\n";
        conf_in << KV_KEY_LONGITUDE " = \n" << KV_KEY_ALTITUDE "=1000 \n;ghsgd";
        Properties prop = ConfigReader(conf_in).read();
        assertEqStr(prop.get_property(SECT_KEY_FALLBACK "." KV_KEY_LATITUDE, "invalid"),
                    "0.000000");
        assertEqStr(prop.get_property(SECT_KEY_FALLBACK "." KV_KEY_LONGITUDE, "invalid"), "invalid");
        assertEqStr(prop.get_property(SECT_KEY_FALLBACK "." KV_KEY_ALTITUDE, "invalid"), "1000");
        assertEqStr(prop.get_property(SECT_KEY_FALLBACK "." KV_KEY_GEOID, "invalid"), "invalid");
        assertTrue(prop.get_property("nothing", "").empty());
    });

    describe<Configuration>("initialize configuration", runner)
        ->test("valid config - full, one feed",
               [] {
                   std::stringstream conf_in;
                   conf_in << "[" SECT_KEY_GENERAL "]\n" << KV_KEY_FEEDS "=" SECT_KEY_ATMOS "1\n";
                   conf_in << KV_KEY_SERVER_PORT "=1234\n" << KV_KEY_GND_MODE "=y\n";
                   conf_in << "[" SECT_KEY_FALLBACK "]\n" << KV_KEY_LATITUDE "=77.777777\n";
                   conf_in << KV_KEY_LONGITUDE "=-12.121212\n" << KV_KEY_ALTITUDE "=1234\n";
                   conf_in << KV_KEY_GEOID "=40.4\n" << KV_KEY_PRESSURE "=999.9\n";
                   conf_in << "[" SECT_KEY_FILTER "]\n" << KV_KEY_MAX_HEIGHT "=-1\n";
                   conf_in << KV_KEY_MAX_DIST "=10000\n";
                   conf_in << "[" SECT_KEY_ATMOS "1]\n" << KV_KEY_HOST "=localhost\n";
                   conf_in << KV_KEY_PORT "=3456\n" << KV_KEY_PRIORITY "=1\n";
                   Configuration config(conf_in);
                   const auto    feed_it = config.get_feedProperties().cbegin();
                   assertEqStr(feed_it->first, SECT_KEY_ATMOS "1");
                   assertEqStr(feed_it->second.get_property(KV_KEY_PRIORITY), "1");
                   assertT(config.get_serverPort(), EQUALS, 1234, int);
                   assertTrue(config.get_groundMode());
                   assertEquals(config.get_position().get_position().latitude, 77.777777);
                   assertEquals(config.get_position().get_position().longitude, -12.121212);
                   assertEquals(config.get_position().get_position().altitude, 1234);
                   assertEquals(config.get_position().get_geoid(), 40.4);
                   assertEquals(config.get_atmPressure(), 999.9);
                   assertEquals(config.get_maxHeight(), INT32_MAX);
                   assertEquals(config.get_maxDistance(), 10000);
               })
        ->test("only valid feeds", [] {
            std::stringstream conf_in;
            conf_in << "[" SECT_KEY_GENERAL "]\n"
                    << KV_KEY_FEEDS "=" SECT_KEY_WIND "," SECT_KEY_SBS "1 , " SECT_KEY_SBS
                                    "2, else,,\n";
            conf_in << "[" SECT_KEY_WIND "]\n"
                    << KV_KEY_HOST "=127.0.0.1\n"
                    << KV_KEY_PORT "=3333\n"
                    << KV_KEY_PRIORITY "=0\n";
            conf_in << "[" SECT_KEY_SBS "1]\n"
                    << KV_KEY_HOST "=127.0.0.1\n"
                    << KV_KEY_PORT "=3334\n"
                    << KV_KEY_PRIORITY "=1\n";
            Configuration config(conf_in);
            std::string   valid(SECT_KEY_WIND "," SECT_KEY_SBS "1,,");
            std::string   result;
            for (const auto& it : config.get_feedNames())
            {
                result += it + ",";
            }
            assertEquals(result, valid);
        });
}

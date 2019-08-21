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

#include "config/ConfigReader.h"
#include "config/Configuration.h"
#include "config/Properties.h"

#include "helper.hpp"

using namespace config;
using namespace sctf;

TEST_MODULE(test_config_reader, {
    test("read config", [] {
        std::stringstream conf_in;
        conf_in << "[" << Configuration::SECT_KEY_FALLBACK << "]\n"
                << Configuration::KV_KEY_LATITUDE << "   = 0.000000\n";
        conf_in << Configuration::KV_KEY_LONGITUDE << " = \n"
                << Configuration::KV_KEY_ALTITUDE << "=1000 \n;ghsgd";
        Properties prop = ConfigReader(conf_in).read();
        assertEqStr(prop.get_property(Configuration::PATH_LATITUDE, "invalid"), "0.000000");
        assertEqStr(prop.get_property(Configuration::PATH_LONGITUDE, "invalid"), "invalid");
        assertEqStr(prop.get_property(Configuration::PATH_ALTITUDE, "invalid"), "1000");
        assertEqStr(prop.get_property(Configuration::PATH_GEOID, "invalid"), "invalid");
        assertTrue(prop.get_property("nothing", "").empty());
    });
})

TEST_MODULE_PARALLEL(test_configuration, {
    test("valid config - full, one feed", [] {
        std::stringstream conf_in;
        conf_in << "[" << Configuration::SECT_KEY_GENERAL << "]\n"
                << Configuration::KV_KEY_FEEDS << "=" << Configuration::SECT_KEY_ATMOS << "1\n";
        conf_in << Configuration::KV_KEY_SERVER_PORT << "=1234\n" << Configuration::KV_KEY_GND_MODE << "=y\n";
        conf_in << "[" << Configuration::SECT_KEY_FALLBACK << "]\n"
                << Configuration::KV_KEY_LATITUDE << "=77.777777\n";
        conf_in << Configuration::KV_KEY_LONGITUDE << "=-12.121212\n"
                << Configuration::KV_KEY_ALTITUDE << "=1234\n";
        conf_in << Configuration::KV_KEY_GEOID << "=40.4\n" << Configuration::KV_KEY_PRESSURE << "=999.9\n";
        conf_in << "[" << Configuration::SECT_KEY_FILTER << "]\n"
                << Configuration::KV_KEY_MAX_HEIGHT << "=-1\n";
        conf_in << Configuration::KV_KEY_MAX_DIST << "=10000\n";
        conf_in << "[" << Configuration::SECT_KEY_ATMOS << "1]\n"
                << Configuration::KV_KEY_HOST << "=localhost\n";
        conf_in << Configuration::KV_KEY_PORT << "=3456\n" << Configuration::KV_KEY_PRIORITY << "=1\n";
        Configuration config(conf_in);
        const auto    feed_it = config.feedProperties.cbegin();
        assertEqStr(feed_it->first, std::string(Configuration::SECT_KEY_ATMOS) + "1");
        assertEqStr(feed_it->second.get_property(Configuration::KV_KEY_PRIORITY), "1");
        assertT(config.serverPort, EQUALS, 1234, int);
        assertTrue(config.groundMode);
        assertEquals(config.gpsPosition.getLocation().latitude, 77.777777);
        assertEquals(config.gpsPosition.getLocation().longitude, -12.121212);
        assertEquals(config.gpsPosition.getLocation().altitude, 1234);
        assertEquals(config.gpsPosition.getGeoid(), 40.4);
        assertEquals(config.atmPressure, 999.9);
        assertEquals(config.maxHeight, INT32_MAX);
        assertEquals(config.maxDistance, 10000);
    });
    test("only valid feeds", [] {
        std::stringstream conf_in;
        conf_in << "[" << Configuration::SECT_KEY_GENERAL << "]\n"
                << Configuration::KV_KEY_FEEDS << "=" << Configuration::SECT_KEY_WIND << ","
                << Configuration::SECT_KEY_SBS << "1 , " << Configuration::SECT_KEY_SBS << "2, else,,\n";
        conf_in << "[" << Configuration::SECT_KEY_WIND << "]\n"
                << Configuration::KV_KEY_HOST << "=127.0.0.1\n"
                << Configuration::KV_KEY_PORT << "=3333\n"
                << Configuration::KV_KEY_PRIORITY << "=0\n";
        conf_in << "[" << Configuration::SECT_KEY_SBS << "1]\n"
                << Configuration::KV_KEY_HOST << "=127.0.0.1\n"
                << Configuration::KV_KEY_PORT << "=3334\n"
                << Configuration::KV_KEY_PRIORITY << "=1\n";
        Configuration config(conf_in);
        std::string   valid(Configuration::SECT_KEY_WIND);
        valid.append(",").append(Configuration::SECT_KEY_SBS).append(+"1,,");
        std::string result;
        for (const auto& it : config.feedNames)
        {
            result += it + ",";
        }
        assertEquals(result, valid);
    });
})

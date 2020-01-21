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

#include <boost/property_tree/ini_parser.hpp>

#include "config/ConfigReader.h"
#include "config/Configuration.h"
#include "config/Properties.h"
#include "util/string_utils.hpp"

#include "helper.hpp"

using namespace sctf;
using namespace vfrb;
using namespace str_util;
using namespace config;

TEST_MODULE_PAR(test_Properties, {
    test("Property - default value", [] {
        boost::property_tree::ptree t;
        boost::property_tree::read_ini("[main]\nkey=value\n", t);
        CProperties p(std::move(t));
        ASSERT_EQUALS(p.Property("main.key", "default"), "value");
        ASSERT_EQUALS(p.Property("main.nokey", "default"), "default");
        ASSERT_EQUALS(p.Property("nosect.nokey", "default"), "default");
    });
    test("Property", [] {
        boost::property_tree::ptree t;
        boost::property_tree::read_ini("[main]\nkey=value\n", t);
        CProperties p(std::move(t));
        ASSERT_NOTHROW(p.Property("main.key"));
        ASSERT_EQUALS(p.Property("main.key"), "value");
        ASSERT_THROWS(p.Property("main.nokey"), config::error::CPropertyNotFoundError);
        ASSERT_THROWS(p.Property("nosect.nokey"), config::error::CPropertyNotFoundError);
    });
    test("Section", [] {
        boost::property_tree::ptree t;
        boost::property_tree::read_ini("[main]\nkey=value\n", t);
        CProperties p(std::move(t));
        CProperties c;
        ASSERT_NOTHROW(c = p.Section("main"));
        ASSERT_EQUALS(c.Property("key"), "value");
        ASSERT_THROWS(p.Section("nosect"), config::error::CPropertyNotFoundError);
    });
})

TEST_MODULE_PAR(test_ConfigReader, {
    test("read config success", [] {
        std::stringstream conf_in;
        conf_in << "[" << CConfiguration::SECT_KEY_FALLBACK << "]\n"
                << CConfiguration::KV_KEY_LATITUDE << "   = 0.000000\n";
        conf_in << CConfiguration::KV_KEY_LONGITUDE << " = \n"
                << CConfiguration::KV_KEY_ALTITUDE << "=1000 \n;ghsgd";
        CProperties p;
        ASSERT_NOTHROW(p = CConfigReader(conf_in).Read());
        ASSERT_EQUALS(p.Property(CConfiguration::PATH_LATITUDE, "invalid"), "0.000000");
        ASSERT_EQUALS(p.Property(CConfiguration::PATH_LONGITUDE, "invalid"), "invalid");
        ASSERT_EQUALS(p.Property(CConfiguration::PATH_ALTITUDE, "invalid"), "1000");
        ASSERT_EQUALS(p.Property(CConfiguration::PATH_GEOID, "invalid"), "invalid");
        ASSERT_TRUE(p.Property("nothing", "").empty());
    });
    test("read config fail", [] {
        std::stringstream conf_in;
        conf_in << "" << CConfiguration::SECT_KEY_FALLBACK << "]\n"
                << CConfiguration::KV_KEY_LATITUDE << "    0.000000\n";
        conf_in << CConfiguration::KV_KEY_LONGITUDE << " = \n"
                << CConfiguration::KV_KEY_ALTITUDE << "=1000 \n;ghsgd";
        ASSERT_THROWS(CConfigReader(conf_in).Read(), config::error::CReadFileError);
    });
})

TEST_MODULE_PAR(test_Configuration, {
    test("valid config - full, one feed", [] {
        std::stringstream conf_in;
        conf_in << "[" << CConfiguration::SECT_KEY_GENERAL << "]\n"
                << CConfiguration::KV_KEY_FEEDS << "=" << CConfiguration::SECT_KEY_ATMOS << "1\n"
                << CConfiguration::KV_KEY_GND_MODE << "=y\n";
        conf_in << "[" << CConfiguration::SECT_KEY_SERVER << "]\n"
                << CConfiguration::KV_KEY_PORT << "=1234\n"
                << CConfiguration::KV_KEY_MAX_CON << "=1\n";
        conf_in << "[" << CConfiguration::SECT_KEY_FALLBACK << "]\n"
                << CConfiguration::KV_KEY_LATITUDE << "=77.777777\n";
        conf_in << CConfiguration::KV_KEY_LONGITUDE << "=-12.121212\n"
                << CConfiguration::KV_KEY_ALTITUDE << "=1234\n";
        conf_in << CConfiguration::KV_KEY_GEOID << "=40.4\n" << CConfiguration::KV_KEY_PRESSURE << "=999.9\n";
        conf_in << "[" << CConfiguration::SECT_KEY_FILTER << "]\n"
                << CConfiguration::KV_KEY_MAX_HEIGHT << "=-1\n";
        conf_in << CConfiguration::KV_KEY_MAX_DIST << "=10000\n";
        conf_in << "[" << CConfiguration::SECT_KEY_ATMOS << "1]\n"
                << CConfiguration::KV_KEY_HOST << "=localhost\n";
        conf_in << CConfiguration::KV_KEY_PORT << "=3456\n" << CConfiguration::KV_KEY_PRIORITY << "=1\n";
        CConfiguration conf(conf_in);
        const auto     feed_it = conf.FeedProperties.cbegin();
        ASSERT_EQUALS(feed_it->first, MakeStr(CConfiguration::SECT_KEY_ATMOS, "1"));
        ASSERT_EQUALS(feed_it->second.Property(CConfiguration::KV_KEY_PRIORITY), "1");
        ASSERT_EQUALS(std::get<0>(conf.ServerConfig), 1234);
        ASSERT_EQUALS(std::get<1>(conf.ServerConfig), 1);
        ASSERT_TRUE(conf.GroundMode);
        ASSERT_EQUALS(conf.GpsPosition.Location().Latitude, 77.777777);
        ASSERT_EQUALS(conf.GpsPosition.Location().Longitude, -12.121212);
        ASSERT_EQUALS(conf.GpsPosition.Location().Altitude, 1234);
        ASSERT_EQUALS(conf.GpsPosition.Geoid(), 40.4);
        ASSERT_EQUALS(conf.AtmPressure, 999.9);
        ASSERT_EQUALS(conf.MaxHeight, INT32_MAX);
        ASSERT_EQUALS(conf.MaxDistance, 10000);
    });
    test("only valid feeds", [] {
        std::stringstream conf_in;
        conf_in << "[" << CConfiguration::SECT_KEY_GENERAL << "]\n"
                << CConfiguration::KV_KEY_FEEDS << "=" << CConfiguration::SECT_KEY_WIND << ","
                << CConfiguration::SECT_KEY_SBS << "1 , " << CConfiguration::SECT_KEY_SBS << "2, else,,\n";
        conf_in << "[" << CConfiguration::SECT_KEY_WIND << "]\n"
                << CConfiguration::KV_KEY_HOST << "=127.0.0.1\n"
                << CConfiguration::KV_KEY_PORT << "=3333\n"
                << CConfiguration::KV_KEY_PRIORITY << "=0\n";
        conf_in << "[" << CConfiguration::SECT_KEY_SBS << "1]\n"
                << CConfiguration::KV_KEY_HOST << "=127.0.0.1\n"
                << CConfiguration::KV_KEY_PORT << "=3334\n"
                << CConfiguration::KV_KEY_PRIORITY << "=1\n";
        CConfiguration conf(conf_in);
        Str         valid = MakeStr(CConfiguration::SECT_KEY_WIND, ",", CConfiguration::SECT_KEY_SBS, "1,,");
        std::string result;
        for (auto const& it : conf.FeedNames)
        {
            result += it + ",";
        }
        ASSERT_EQUALS(result, valid);
    });
})

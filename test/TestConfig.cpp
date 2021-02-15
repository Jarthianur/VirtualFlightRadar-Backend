/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>

#include <boost/property_tree/ini_parser.hpp>

#include "config/CConfigReader.hpp"
#include "config/CConfiguration.hpp"
#include "config/CProperties.hpp"
#include "util/StringUtils.hpp"

#include "Helper.hpp"

using namespace vfrb;
using namespace str_util;
using namespace config;

DESCRIBE("test_CProperties") {
    SPtr<CProperties> uut;

    SETUP() {
        boost::property_tree::ptree tree;
        std::stringstream           iss;
        iss << "[main]\nkey=value\n";
        boost::property_tree::read_ini(iss, tree);
        uut = std::make_shared<CProperties>(std::move(tree));
    }

    // Property
    IT("should return the correct value") {
        ASSERT_EQ(uut->Property("main.key"), "value");
    }
    IT("should return the default value if key does not exist") {
        ASSERT_EQ(uut->Property("main.nokey", "default"), "default");
    }
    IT("should return the default value if section does not exist") {
        ASSERT_EQ(uut->Property("nosect.nokey", "default"), "default");
    }
    IT("should not throw if key exists") {
        ASSERT_NOTHROW(uut->Property("main.key"));
    }
    IT("should throw if key does not exist") {
        ASSERT_THROWS(uut->Property("main.nokey"), config::error::CPropertyNotFoundError);
        ASSERT_THROWS(uut->Property("nosect.nokey"), config::error::CPropertyNotFoundError);
    }

    // Section
    IT("should return nested properties as section") {
        ASSERT_EQ(uut->Section("main").Property("key"), "value");
    }
    IT("should not throw if section exists") {
        ASSERT_NOTHROW(uut->Section("main"));
    }
    IT("should throw if section does not exist") {
        ASSERT_THROWS(uut->Section("nosect"), config::error::CPropertyNotFoundError);
    };
};

DESCRIBE("test_CConfigReader") {
    std::stringstream validConf;

    BEFORE_EACH() {
        validConf.str("");
        validConf.clear();
        validConf << "[" << CConfiguration::SECT_KEY_FALLBACK << "]\n"
                  << CConfiguration::KV_KEY_LATITUDE << "   = 0.000000\n"
                  << CConfiguration::KV_KEY_LONGITUDE << " = \n"
                  << CConfiguration::KV_KEY_ALTITUDE << "=1000 \n;ghsgd";
    }

    // valid
    IT("should not throw with valid config") {
        ASSERT_NOTHROW(CConfigReader(validConf).Read());
    }
    IT("should read existing values correctly") {
        CProperties p(CConfigReader(validConf).Read());
        ASSERT_EQ(p.Property(CConfiguration::PATH_LATITUDE, "invalid"), "0.000000");
        ASSERT_EQ(p.Property(CConfiguration::PATH_ALTITUDE, "invalid"), "1000");
    }
    IT("should return default value if key does not exist, or is empty") {
        CProperties p(CConfigReader(validConf).Read());
        ASSERT_EQ(p.Property(CConfiguration::PATH_LONGITUDE, "invalid"), "invalid");
        ASSERT_EQ(p.Property(CConfiguration::PATH_GEOID, "invalid"), "invalid");
        ASSERT_TRUE(p.Property("nothing", "").empty());
    }

    // invalid
    IT("should throw if config is invalid") {
        std::stringstream conf_in;
        conf_in << CConfiguration::SECT_KEY_FALLBACK << "\n"
                << CConfiguration::KV_KEY_LATITUDE << "    0.000000\n";
        ASSERT_THROWS(CConfigReader(conf_in).Read(), config::error::CReadFileError);
    };
};

DESCRIBE_PAR("test_CConfiguration") {
    SPtr<CConfiguration> uut;

    SETUP() {
        std::stringstream conf_in;
        // general
        conf_in << "[" << CConfiguration::SECT_KEY_GENERAL << "]\n"
                << CConfiguration::KV_KEY_FEEDS << "=" << CConfiguration::SECT_KEY_ATMOS << "1,"
                << CConfiguration::SECT_KEY_WIND << "," << CConfiguration::SECT_KEY_SBS << "1 , "
                << CConfiguration::SECT_KEY_SBS << "2, else,,\n"
                << CConfiguration::KV_KEY_GND_MODE << "=y\n";
        // server
        conf_in << "[" << CConfiguration::SECT_KEY_SERVER << "]\n"
                << CConfiguration::KV_KEY_PORT << "=1234\n"
                << CConfiguration::KV_KEY_MAX_CON << "=1\n";
        // fallback
        conf_in << "[" << CConfiguration::SECT_KEY_FALLBACK << "]\n"
                << CConfiguration::KV_KEY_LATITUDE << "=77.777777\n"
                << CConfiguration::KV_KEY_LONGITUDE << "=-12.121212\n"
                << CConfiguration::KV_KEY_ALTITUDE << "=1234\n"
                << CConfiguration::KV_KEY_GEOID << "=40.4\n"
                << CConfiguration::KV_KEY_PRESSURE << "=999.9\n";
        // filter
        conf_in << "[" << CConfiguration::SECT_KEY_FILTER << "]\n"
                << CConfiguration::KV_KEY_MAX_HEIGHT << "=-1\n"
                << CConfiguration::KV_KEY_MAX_DIST << "=10000\n";
        // atm1
        conf_in << "[" << CConfiguration::SECT_KEY_ATMOS << "1]\n"
                << CConfiguration::KV_KEY_HOST << "=localhost\n"
                << CConfiguration::KV_KEY_PORT << "=3456\n"
                << CConfiguration::KV_KEY_PRIORITY << "=1\n";
        // wind
        conf_in << "[" << CConfiguration::SECT_KEY_WIND << "]\n"
                << CConfiguration::KV_KEY_HOST << "=127.0.0.1\n"
                << CConfiguration::KV_KEY_PORT << "=3333\n"
                << CConfiguration::KV_KEY_PRIORITY << "=0\n";
        // sbs1
        conf_in << "[" << CConfiguration::SECT_KEY_SBS << "1]\n"
                << CConfiguration::KV_KEY_HOST << "=127.0.0.1\n"
                << CConfiguration::KV_KEY_PORT << "=3334\n"
                << CConfiguration::KV_KEY_PRIORITY << "=1\n";

        uut = std::make_shared<CConfiguration>(conf_in);
    }

    IT("should hold correct values for general") {
        ASSERT_TRUE(uut->GroundMode);
    }
    IT("should hold correct values for server") {
        ASSERT_EQ(std::get<0>(uut->ServerConfig), 1234U);
        ASSERT_EQ(std::get<1>(uut->ServerConfig), 1U);
    }
    IT("should hold correct values for fallback") {
        ASSERT(uut->GpsPosition.Location().Latitude, EQ, 77.777777);
        ASSERT(uut->GpsPosition.Location().Longitude, EQ, -12.121212);
        ASSERT_EQ(uut->GpsPosition.Location().Altitude, 1234);
        ASSERT(uut->GpsPosition.Geoid(), EQ, 40.4);
        ASSERT(uut->AtmPressure, EQ, 999.9);
    }
    IT("should hold correct values for filter") {
        ASSERT_EQ(uut->MaxHeight, INT32_MAX);
        ASSERT_EQ(uut->MaxDistance, 10000);
    }
    IT("should hold correct values for atm1") {
        const auto feed_it = uut->FeedProperties.find(MakeStr(CConfiguration::SECT_KEY_ATMOS, "1"));
        ASSERT_EQ(feed_it->first, MakeStr(CConfiguration::SECT_KEY_ATMOS, "1"));
        ASSERT_EQ(feed_it->second.Property(CConfiguration::KV_KEY_PRIORITY), "1");
    }
};

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

#define VFRB_TEST_MODE

#include "boost/property_tree/ini_parser.hpp"
#include "data/CAircraftData.hpp"
#include "feed/CSbsFeed.hpp"

#include "Helper.hpp"

using namespace vfrb;
using feed::CSbsFeed;
using config::CProperties;

DESCRIBE("test_CSbsFeed") {
    Shared<data::CAircraftData> data;
    Shared<CProperties>         prop;
    Shared<CSbsFeed>            uut;

    SETUP() {
        boost::property_tree::ptree tree;
        std::stringstream           iss;
        iss << "host=localhost\nport=1234\npriority=0\n";
        boost::property_tree::read_ini(iss, tree);
        prop = AllocShared<CProperties>(std::move(tree));
    }

    BEFORE_EACH() {
        data = AllocShared<data::CAircraftData>();
        uut  = AllocShared<CSbsFeed>("sbsuut", *prop, data, 999999);
    }

    IT("should initialize correctly") {
        ASSERT(uut->Protocol(), EQ, feed::IFeed::EProtocol::SBS);
        ASSERT(uut->Name(), EQ, "sbsuut");
        ASSERT(uut->Priority(), EQ, 0);
        ASSERT(uut->Endpoint(), EQ, client::net::SEndpoint{"localhost", "1234"});
    }

    IT("should process a full update for single aircraft") {
        ASSERT_EQ(data->Size(), 0);
        uut->Consume(
            "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0");
        uut->Await();
        ASSERT_EQ(data->Size(), 0);
        uut->Consume(
            "MSG,4,0,0,AAAAAA,0,2017/02/16,20:11:30.800,2017/02/16,20:11:30.800,,,100.0,180.0,,,64,,,,,");
        uut->Await();
        ASSERT_EQ(data->Size(), 1);
    }

    IT("should process a single update for multiple aircrafts") {
        ASSERT_EQ(data->Size(), 0);
        uut->Consume(
            "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0");
        uut->Await();
        ASSERT_EQ(data->Size(), 0);
        uut->Consume(
            "MSG,4,0,0,BBBBBB,0,2017/02/16,20:11:30.800,2017/02/16,20:11:30.800,,,100.0,180.0,,,64,,,,,");
        uut->Await();
        ASSERT_EQ(data->Size(), 0);
        uut->Consume(
            "MSG,3,0,0,BBBBBB,0,2017/02/16,20:11:30.900,2017/02/16,20:11:30.900,,1000,,,49.000000,8.000000,,,,,,0");
        uut->Await();
        ASSERT_EQ(data->Size(), 1);
        uut->Consume(
            "MSG,4,0,0,AAAAAA,0,2017/02/16,20:11:30.900,2017/02/16,20:11:30.900,,,100.0,180.0,,,64,,,,,");
        uut->Await();
        ASSERT_EQ(data->Size(), 2);
    }

    IT("should process a full update for single aircraft independent of order") {
        ASSERT_EQ(data->Size(), 0);
        uut->Consume(
            "MSG,4,0,0,AAAAAA,0,2017/02/16,20:11:30.800,2017/02/16,20:11:30.800,,,100.0,180.0,,,64,,,,,");
        uut->Await();
        ASSERT_EQ(data->Size(), 0);
        uut->Consume(
            "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0");
        uut->Await();
        ASSERT_EQ(data->Size(), 1);
    }

    IT("should process a full update with multiple messages") {
        ASSERT_EQ(data->Size(), 0);
        uut->Consume(
            "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0");
        uut->Await();
        ASSERT_EQ(data->Size(), 0);
        uut->Consume(
            "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.790,2017/02/16,20:11:30.790,,2000,,,49.000000,8.000000,,,,,,0");
        uut->Await();
        ASSERT_EQ(data->Size(), 0);
        uut->Consume(
            "MSG,4,0,0,AAAAAA,0,2017/02/16,20:11:30.800,2017/02/16,20:11:30.800,,,100.0,180.0,,,64,,,,,");
        uut->Await();
        ASSERT_EQ(data->Size(), 1);
        auto res = data->Get("AAAAAA");
        ASSERT_EQ(res.Location().Altitude, 2000);
    };
};
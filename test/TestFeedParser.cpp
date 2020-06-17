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

#include "feed/parser/CAprsParser.hpp"
#include "feed/parser/CAtmosphereParser.hpp"
#include "feed/parser/CGpsParser.hpp"
#include "feed/parser/CSbsParser.hpp"
#include "feed/parser/CWindParser.hpp"
#include "math/Math.hpp"
#include "object/CAircraft.hpp"
#include "object/CAtmosphere.hpp"
#include "object/CGpsPosition.hpp"
#include "object/CTimestamp.hpp"
#include "object/CWind.hpp"

#include "Helper.hpp"

using sctf::FEQ;

using namespace vfrb;
using namespace feed::parser;

DESCRIBE_PAR("test_CSbsParser") {
    IT("should parse a valid sentence correctly") {
        str msg =
            "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0";
        CSbsParser p(100000);
        ASSERT_NOTHROW(p.Parse(msg, 0));
        auto ac = p.Parse(msg, 0);
        ASSERT_EQ(ac.Id(), "AAAAAA");
        ASSERT_EQ(ac.TargetType(), object::CAircraft::ETargetType::TRANSPONDER);
        ASSERT_EQ(ac.Location().Altitude, math::DoubleToInt(math::FEET_2_M * 1000));
        ASSERT(ac.Location().Latitude, FEQ(), 49.0);
        ASSERT(ac.Location().Longitude, FEQ(), 8.0);
        ASSERT_FALSE(ac.HasFullInfo());
    }
    IT("should throw when parsing invalid sentence") {
        CSbsParser p(100000);
        ASSERT_THROWS(
            p.Parse("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,,,,,,,,,,,0", 0),
            feed::parser::error::CParseError);
        ASSERT_THROWS(
            p.Parse("MSG,3,0,0,,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,,,,,,,,0", 0),
            feed::parser::error::CParseError);
        ASSERT_THROWS(
            p.Parse(
                "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,,,,,,,0",
                0),
            feed::parser::error::CParseError);
        ASSERT_THROWS(p.Parse("MSG,someCrap in, here", 0), feed::parser::error::CParseError);
        ASSERT_THROWS(p.Parse("MSG,4,0,,,,,,", 0), feed::parser::error::CParseError);
        ASSERT_THROWS(
            p.Parse("MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0",
                    0),
            feed::parser::error::CParseError);
        ASSERT_THROWS(
            p.Parse("MSG,3,0,0,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,100#0,,,,,,,,,,0", 0),
            feed::parser::error::CParseError);
        ASSERT_THROWS(p.Parse("", 0), feed::parser::error::CParseError);
    }
    IT("should filter by height") {
        CSbsParser p(0);
        ASSERT_THROWS(
            p.Parse(
                "MSG,3,0,0,AAAAAA,0,2017/02/16,20:11:30.772,2017/02/16,20:11:30.772,,1000,,,49.000000,8.000000,,,,,,0",
                0),
            feed::parser::error::CParseError);
    };
};

DESCRIBE_PAR("test_CAprsParser") {
    IT("should parse a valid sentence correctly") {
        CAprsParser p(10000);
        ASSERT_NOTHROW(
            p.Parse("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 !W19! id06AAAAAA", 0));
        ASSERT_NOTHROW(p.Parse(
            "ICAAAAAAA>APRS,qAR:/081733h4900.00N/00800.00EX180/003/A=000000 !W38! id0DAAAAAA -138fpm +0.0rot 6.2dB 0e +4.2kHz gps4x4",
            0));
        ASSERT_NOTHROW(p.Parse(
            "FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00S\\00800.00E^276/014/A=000000 !W07! id22AAAAAA -019fpm +3.7rot 37.8dB 0e -51.2kHz gps2x4",
            0));
        ASSERT_NOTHROW(p.Parse(
            "FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=000000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz",
            0));
        auto ac = p.Parse(
            "FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=000000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz",
            0);
        ASSERT_EQ(ac.Id(), "AAAAAA");
        ASSERT_EQ(ac.TargetType(), object::CAircraft::ETargetType::FLARM);
        ASSERT_EQ(ac.Location().Altitude, 0);
        ASSERT(ac.Location().Latitude, FEQ(), 49.0);
        ASSERT(ac.Location().Longitude, FEQ(), -8.0);
        ASSERT_TRUE(ac.HasFullInfo());
    }
    IT("should throw when parsing invalid sentence") {
        CAprsParser p(10000);
        ASSERT_THROWS(
            p.Parse(
                "Valhalla>APRS,TCPIP*,qAC,GLIDERN2:/074555h4900.00NI00800.00E&/A=000000 CPU:4.0 RAM:242.7/458.8MB NTP:0.8ms/-28.6ppm +56.2C RF:+38+2.4ppm/+1.7dB",
                0),
            feed::parser::error::CParseError);
        ASSERT_THROWS(
            p.Parse("# aprsc 2.0.14-g28c5a6a 29 Jun 2014 07:46:15 GMT SERVER1 00.000.00.000:14580", 0),
            feed::parser::error::CParseError);
        ASSERT_THROWS(p.Parse("", 0), feed::parser::error::CParseError);
        ASSERT_THROWS(p.Parse("FLRAAAAAA>APRS,qAS,XXXX:/100715h4900.00N/00800.00E'/A=000000 ", 0),
                      feed::parser::error::CParseError);
    }
    IT("should filter by height") {
        CAprsParser p(0);
        ASSERT_THROWS(
            p.Parse(
                "FLRAAAAAA>APRS,qAS,XXXX:/074548h4900.00N/00800.00W'000/000/A=001000 id0AAAAAAA +000fpm +0.0rot 5.5dB 3e -4.3kHz",
                0),
            feed::parser::error::CParseError);
    };
};

TEST_MODULE(test_wind_parser, {
    test("valid msg", [] {
        WindParser   windParser;
        object::Wind wind;
        std::string  mwv("$WIMWV,242.8,R,6.9,N,A*20\r\n");
        assertTrue(windParser.unpack(mwv, wind));
        ASSERT_EQ(wind.get_serialized(), mwv);
    });
    test("invalid msg", [] {
        WindParser   windParser;
        object::Wind wind;
        assertFalse(windParser.unpack("$YXXDR,C,19.3,C,BRDT,U,11.99,V,BRDV*75", wind));
        assertFalse(windParser.unpack("Someone sent other stuff", wind));
        assertFalse(windParser.unpack("", wind));
    });
})

TEST_MODULE(test_atmosphere_parser, {
    test("valid msg", [] {
        AtmosphereParser   atmParser;
        object::Atmosphere atm;
        std::string        mda("$WIMDA,29.7987,I,1.0091,B,14.8,C,,,,,,,,,,,,,,*3E\r\n");
        assertTrue(atmParser.unpack(mda, atm));
        ASSERT_EQ(atm.get_pressure(), 1009.1);
        ASSERT_EQ(atm.get_serialized(), mda);
    });
    test("invalid msg", [] {
        AtmosphereParser   atmParser;
        object::Atmosphere atm;
        assertFalse(atmParser.unpack("$YXXDR,C,19.3,C,BRDT,U,11.99,V,BRDV*75", atm));
        assertFalse(atmParser.unpack("Someone sent other stuff", atm));
        assertFalse(atmParser.unpack("$WIMDA,29.7987,I,1.0091,14.8,,,,,,,,,,,,,,*3F", atm));
        assertFalse(atmParser.unpack("$WIMDA,", atm));
        assertFalse(atmParser.unpack("$WIMDA,29.7987,I,1.0#091,B,14.8,C,,,,,,,,,,,,,,*1d", atm));
        assertFalse(atmParser.unpack("", atm));
    });
})

TEST_MODULE(test_gps_parser, {
    test("valid msg", [] {
        GpsParser           gpsParser;
        object::GpsPosition pos;
        assertTrue(gpsParser.unpack("$GPGGA,183552,5000.0466,N,00815.7555,E,1,05,1,105,M,48.0,M,,*49", pos));
        assertTrue(
            gpsParser.unpack("$GPGGA,183552,5000.0466,N,00815.7555,E,1,05,1,105,M,48.0,M,,*49\r\n", pos));
        assertTrue(
            gpsParser.unpack("$GPGGA,183552,5000.0466,S,00815.7555,W,1,05,1,105,M,48.0,M,,*46\n", pos));
        ASSERT_EQ(pos.get_dilution(), 1.0);
        ASSERT_EQ(pos.get_fixQuality(), 1);
        ASSERT_EQ(pos.get_nrOfSatellites(), 5);
        ASSERT_EQ(pos.get_geoid(), 48.0);
        ASSERT_EQ(pos.get_position().latitude, -math::dmToDeg(5000.0466));
        ASSERT_EQ(pos.get_position().longitude, -math::dmToDeg(815.7555));
        ASSERT_EQ(pos.get_position().altitude, 105);
    });
    test("invalid msg", [] {
        GpsParser           gpsParser;
        object::GpsPosition pos;
        assertFalse(
            gpsParser.unpack("$GPGGA,183552,5000.0466,N,00815.7555,E,1,05,1,105,M,48.0,M,,*59\r\n", pos));
        assertFalse(gpsParser.unpack("$GPGGA,183552,N,00815.7555,E,1,05,1,105,M,48.0,M,,*59\r\n", pos));
        assertFalse(gpsParser.unpack("$GPGGA,\r\n", pos));
        assertFalse(gpsParser.unpack("", pos));
        assertFalse(gpsParser.unpack("$GPGGA,183552,N,00815.7555,E,1,05,1,105,M,48.0,M,,*\r\n", pos));
    });
})

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

#include "util/string_utils.hpp"
#include "util/utility.hpp"

#include "helper.hpp"

using namespace sctf;
using namespace vfrb;
using namespace util;
using namespace str_util;

void test_utility()
{
    suite("utility")->test("FailOutsideBounds", [] {
        ASSERT_NOTHROW(FailOutsideBounds(1., 2., 3.));
        ASSERT_THROWS(FailOutsideBounds(0., 2., 3.), util::error::CLimitsExceededError);
    });
}

void test_string_utils()
{
    suite_par("string_utils")
        ->test("Checksum",
               [] {
                   ASSERT_EQUALS(Checksum("", 0), 0);
                   ASSERT_EQUALS(Checksum("\0", 0), 0);
                   ASSERT_EQUALS(Checksum("$abc*", 0), 96);
                   ASSERT_EQUALS(Checksum("$abc*", 6), 0);
               })
        ->test("MatchChecksum",
               [] {
                   ASSERT_TRUE(MatchChecksum("$abc*60"));
                   ASSERT_FALSE(MatchChecksum("xyz"));
                   ASSERT_FALSE(MatchChecksum("$abc*33"));
                   ASSERT_FALSE(MatchChecksum("$abc*xy"));
               })
        ->test("Convert (f64)",
               [] {
                   {
                       char const* nr  = "1234.0";
                       char const* nr2 = "-1234.0";
                       auto [v, e]     = Convert<f64>(nr, nr + 6);
                       auto [v2, e2]   = Convert<f64>(nr2, nr2 + 7);
                       ASSERT_EQUALS(e, EErrc::OK);
                       ASSERT_EQUALS(v, 1234.0);
                       ASSERT_EQUALS(e2, EErrc::OK);
                       ASSERT_EQUALS(v2, -1234.0);
                   }
                   {
                       char const* noNr = "abc";
                       auto [v, e]      = Convert<f64>(noNr, noNr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* noNr = "";
                       auto [v, e]      = Convert<f64>(noNr, noNr + 1);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* nr = "1";
                       auto [v, e]    = Convert<f64>(nr, nr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
               })
        ->test("Convert (s8)",
               [] {
                   {
                       char const* nr  = "100";
                       char const* nr2 = "-100";
                       auto [v, e]     = Convert<s8>(nr, nr + 3);
                       auto [v2, e2]   = Convert<s8>(nr2, nr2 + 4);
                       ASSERT_EQUALS(e, EErrc::OK);
                       ASSERT_EQUALS(v, 100);
                       ASSERT_EQUALS(e2, EErrc::OK);
                       ASSERT_EQUALS(v2, -100);
                   }
                   {
                       char const* noNr = "abc";
                       auto [v, e]      = Convert<s8>(noNr, noNr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* noNr = "";
                       auto [v, e]      = Convert<s8>(noNr, noNr + 1);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* nr = "1";
                       auto [v, e]    = Convert<s8>(nr, nr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
               })
        ->test("Convert (u8)",
               [] {
                   {
                       char const* nr = "250";
                       auto [v, e]    = Convert<u8>(nr, nr + 3);
                       ASSERT_EQUALS(e, EErrc::OK);
                       ASSERT_EQUALS(v, 250);
                   }
                   {
                       char const* noNr = "abc";
                       auto [v, e]      = Convert<u8>(noNr, noNr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* noNr = "";
                       auto [v, e]      = Convert<u8>(noNr, noNr + 1);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* nr = "1";
                       auto [v, e]    = Convert<u8>(nr, nr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
               })
        ->test("Convert (u16)",
               [] {
                   {
                       char const* nr = "1234";
                       auto [v, e]    = Convert<u16>(nr, nr + 4);
                       ASSERT_EQUALS(e, EErrc::OK);
                       ASSERT_EQUALS(v, 1234);
                   }
                   {
                       char const* noNr = "abc";
                       auto [v, e]      = Convert<u16>(noNr, noNr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* noNr = "";
                       auto [v, e]      = Convert<u16>(noNr, noNr + 1);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* nr = "1";
                       auto [v, e]    = Convert<u16>(nr, nr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
               })
        ->test("Convert (s32)",
               [] {
                   {
                       char const* nr  = "1234";
                       char const* nr2 = "-1234";
                       auto [v, e]     = Convert<s32>(nr, nr + 4);
                       auto [v2, e2]   = Convert<s32>(nr2, nr2 + 5);
                       ASSERT_EQUALS(e, EErrc::OK);
                       ASSERT_EQUALS(v, 1234);
                       ASSERT_EQUALS(e2, EErrc::OK);
                       ASSERT_EQUALS(v2, -1234);
                   }
                   {
                       char const* noNr = "abc";
                       auto [v, e]      = Convert<s32>(noNr, noNr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* noNr = "";
                       auto [v, e]      = Convert<s32>(noNr, noNr + 1);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* nr = "1";
                       auto [v, e]    = Convert<s32>(nr, nr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
               })
        ->test("Convert (u32)",
               [] {
                   {
                       char const* nr = "1234";
                       auto [v, e]    = Convert<u32>(nr, nr + 4);
                       ASSERT_EQUALS(e, EErrc::OK);
                       ASSERT_EQUALS(v, 1234);
                   }
                   {
                       char const* noNr = "abc";
                       auto [v, e]      = Convert<u32>(noNr, noNr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* noNr = "";
                       auto [v, e]      = Convert<u32>(noNr, noNr + 1);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* nr = "1";
                       auto [v, e]    = Convert<u32>(nr, nr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
               })
        ->test("Convert (x32)",
               [] {
                   {
                       char const* nr  = "4";
                       char const* nr2 = "0x4";
                       auto [v, e]     = Convert<x32>(nr, nr + 1);
                       auto [v2, e2]   = Convert<x32>(nr2, nr2 + 3);
                       ASSERT_EQUALS(e, EErrc::OK);
                       ASSERT_EQUALS(v, 4);
                       ASSERT_EQUALS(e2, EErrc::OK);
                       ASSERT_EQUALS(v2, 4);
                   }
                   {
                       char const* noNr = "abc";
                       auto [v, e]      = Convert<x32>(noNr, noNr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* noNr = "";
                       auto [v, e]      = Convert<x32>(noNr, noNr + 1);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* nr = "1";
                       auto [v, e]    = Convert<x32>(nr, nr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
               })
        ->test("Convert (u64)",
               [] {
                   {
                       char const* nr = "1234";
                       auto [v, e]    = Convert<u64>(nr, nr + 4);
                       ASSERT_EQUALS(e, EErrc::OK);
                       ASSERT_EQUALS(v, 1234);
                   }
                   {
                       char const* noNr = "abc";
                       auto [v, e]      = Convert<u64>(noNr, noNr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* noNr = "";
                       auto [v, e]      = Convert<u64>(noNr, noNr + 1);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
                   {
                       char const* nr = "1";
                       auto [v, e]    = Convert<u64>(nr, nr + 3);
                       ASSERT_EQUALS(e, EErrc::ERR);
                   }
               })
        ->test("Parse", [] {
            ASSERT_NOTHROW(Parse<s32>("1234"));
            ASSERT_EQUALS(Parse<s32>("1234"), 1234);
            ASSERT_THROWS(Parse<s32>(""), str_util::error::CConversionError);
        });
}

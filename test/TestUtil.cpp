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

#include <ostream>

#include "util/Bounds.hpp"
#include "util/CHashQueue.hpp"
#include "util/StringUtils.hpp"

#include "Helper.hpp"

using namespace vfrb;
using namespace util;
using namespace str_util;

DESCRIBE_PAR("test_Bounds") {
    IT("should not throw if value is in bounds") {
        ASSERT_NOTHROW(FailOutsideBounds(2., 1., 3.));
    }
    IT("should throw if value is not in bounds") {
        ASSERT_THROWS(FailOutsideBounds(0., 2., 3.), util::error::CBoundsExceededError);
    };
};

DESCRIBE_PAR("test_StringUtils") {
    IT("should compute correct checksum") {
        String s1{};
        String s2{"\0"};
        String s3{"$abc*"};
        ASSERT_EQ(Checksum(s1.begin(), s1.end()), 0U);
        ASSERT_EQ(Checksum(s2.begin(), s2.end()), 0U);
        ASSERT_EQ(Checksum(s3.begin(), s3.end()), 96U);
    }
    IT("should match checksum correctly") {
        ASSERT_TRUE(MatchChecksum("$abc*60"));
        ASSERT_FALSE(MatchChecksum("xyz"));
        ASSERT_FALSE(MatchChecksum("$abc*33"));
        ASSERT_FALSE(MatchChecksum("$abc*xy"));
    }
    // f64
    IT("should convert f64 correctly") {
        str nr        = "1234.0";
        str nr2       = "-1234.0";
        auto [v, e]   = Convert<f64>(nr, nr + 6);
        auto [v2, e2] = Convert<f64>(nr2, nr2 + 7);
        ASSERT_EQ(e, EErrc::OK);
        ASSERT(v, EQ, 1234.0);
        ASSERT_EQ(e2, EErrc::OK);
        ASSERT(v2, EQ, -1234.0);
    }
    IT("should fail to convert bad f64") {
        {
            str noNr    = "abc";
            auto [v, e] = Convert<f64>(noNr, noNr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str noNr    = "";
            auto [v, e] = Convert<f64>(noNr, noNr + 1);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str nr      = "1";
            auto [v, e] = Convert<f64>(nr, nr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
    }
    // s8
    IT("should convert s8 correctly") {
        str nr        = "100";
        str nr2       = "-100";
        auto [v, e]   = Convert<i8>(nr, nr + 3);
        auto [v2, e2] = Convert<i8>(nr2, nr2 + 4);
        ASSERT_EQ(e, EErrc::OK);
        ASSERT_EQ(v, 100);
        ASSERT_EQ(e2, EErrc::OK);
        ASSERT_EQ(v2, -100);
    }
    IT("should fail to convert bad s8") {
        {
            str noNr    = "abc";
            auto [v, e] = Convert<i8>(noNr, noNr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str noNr    = "";
            auto [v, e] = Convert<i8>(noNr, noNr + 1);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str nr      = "1";
            auto [v, e] = Convert<i8>(nr, nr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
    }
    // u8
    IT("should convert u8 correctly") {
        str nr      = "250";
        auto [v, e] = Convert<u8>(nr, nr + 3);
        ASSERT_EQ(e, EErrc::OK);
        ASSERT_EQ(v, 250);
    }
    IT("should fail to convert bad u8") {
        {
            str noNr    = "abc";
            auto [v, e] = Convert<u8>(noNr, noNr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str noNr    = "";
            auto [v, e] = Convert<u8>(noNr, noNr + 1);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str nr      = "1";
            auto [v, e] = Convert<u8>(nr, nr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
    }
    // u16
    IT("should convert u16 correctly") {
        str nr      = "1234";
        auto [v, e] = Convert<u16>(nr, nr + 4);
        ASSERT_EQ(e, EErrc::OK);
        ASSERT_EQ(v, 1234);
    }
    IT("should fail to convert bad u16") {
        {
            str noNr    = "abc";
            auto [v, e] = Convert<u16>(noNr, noNr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str noNr    = "";
            auto [v, e] = Convert<u16>(noNr, noNr + 1);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str nr      = "1";
            auto [v, e] = Convert<u16>(nr, nr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
    }
    // s32
    IT("should convert s32 correctly") {
        str nr        = "1234";
        str nr2       = "-1234";
        auto [v, e]   = Convert<i32>(nr, nr + 4);
        auto [v2, e2] = Convert<i32>(nr2, nr2 + 5);
        ASSERT_EQ(e, EErrc::OK);
        ASSERT_EQ(v, 1234);
        ASSERT_EQ(e2, EErrc::OK);
        ASSERT_EQ(v2, -1234);
    }
    IT("should fail to convert bad s32") {
        {
            str noNr    = "abc";
            auto [v, e] = Convert<i32>(noNr, noNr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str noNr    = "";
            auto [v, e] = Convert<i32>(noNr, noNr + 1);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str nr      = "1";
            auto [v, e] = Convert<i32>(nr, nr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
    }
    // u32
    IT("should convert u32 correctly") {
        str nr      = "1234";
        auto [v, e] = Convert<u32>(nr, nr + 4);
        ASSERT_EQ(e, EErrc::OK);
        ASSERT_EQ(v, 1234U);
    }
    IT("should fail to convert bad u32") {
        {
            str noNr    = "abc";
            auto [v, e] = Convert<u32>(noNr, noNr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str noNr    = "";
            auto [v, e] = Convert<u32>(noNr, noNr + 1);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str nr      = "1";
            auto [v, e] = Convert<u32>(nr, nr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
    }
    // x32
    IT("should convert x32 correctly") {
        str nr        = "4";
        str nr2       = "abc";
        auto [v, e]   = Convert<x32>(nr, nr + 1);
        auto [v2, e2] = Convert<x32>(nr2, nr2 + 3);
        ASSERT_EQ(e, EErrc::OK);
        ASSERT_EQ(v, 4U);
        ASSERT_EQ(e2, EErrc::OK);
        ASSERT_EQ(v2, 2748U);
    }
    IT("should fail to convert bad x32") {
        {
            str noNr    = "xyz";
            auto [v, e] = Convert<x32>(noNr, noNr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str noNr    = "";
            auto [v, e] = Convert<x32>(noNr, noNr + 1);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str nr      = "1";
            auto [v, e] = Convert<x32>(nr, nr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
    }
    // u64
    IT("should convert u64 correctly") {
        str nr      = "1234";
        auto [v, e] = Convert<u64>(nr, nr + 4);
        ASSERT_EQ(e, EErrc::OK);
        ASSERT_EQ(v, 1234U);
    }
    IT("should fail to convert bad u64") {
        {
            str noNr    = "abc";
            auto [v, e] = Convert<u64>(noNr, noNr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str noNr    = "";
            auto [v, e] = Convert<u64>(noNr, noNr + 1);
            ASSERT_EQ(e, EErrc::ERR);
        }
        {
            str nr      = "1";
            auto [v, e] = Convert<u64>(nr, nr + 3);
            ASSERT_EQ(e, EErrc::ERR);
        }
    }

    IT("should parse a number correctly") {
        auto r = ASSERT_NOTHROW(return Parse<i32>("1234"));
        ASSERT_EQ(r, 1234);
    }
    IT("should throw on parse failure") {
        ASSERT_THROWS(Parse<i32>(""), str_util::error::CConversionError);
    };
};

DESCRIBE("test_CHashQueue") {
    Shared<CHashQueue<String, i32>> uut;
    u64                             cleanCount = 0;

    SETUP() {
        uut = AllocShared<CHashQueue<String, i32>>(1, [this] { cleanCount += 1; });
    }

    IT("should call not-found-callback when key is not in queue") {
        i32 o = 0;
        ASSERT_NOTHROW(uut->Push(
            std::make_pair("", 1), [](auto&, auto&) { throw std::runtime_error("wrong callback"); },
            [&o](auto& i_) {
                o = i_.second;
                return i_.second;
            }));
        ASSERT_EQ(o, 1);
    }

    IT("should call found-callback when key is in queue") {
        i32 o = 0;
        ASSERT_NOTHROW(uut->Push(
            std::make_pair("", 2),
            [&o](auto& it_, auto& i_) {
                ASSERT_EQ(it_->first, i_.first);
                ASSERT_EQ(it_->second, 1);
                ASSERT_EQ(i_.second, 2);
                it_->second = i_.second;
                o           = i_.second;
            },
            [](auto&) {
                throw std::runtime_error("wrong callback");
                return 0;
            }));
        ASSERT_EQ(o, 2);
    }

    IT("should clean itself frequently") {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        ASSERT_GT(cleanCount, 0);
        u64 c = cleanCount;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        ASSERT_GT(cleanCount, c);
    }

    IT("should remove an element") {
        uut->RemoveIf([](auto& it_) { return it_->first == ""; });
        ASSERT_NOTHROW(uut->Push(
            std::make_pair("", 1), [](auto&, auto&) { throw std::runtime_error("wrong callback"); },
            [](auto&) { return 1; }));
    };
};
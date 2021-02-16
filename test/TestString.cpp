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

#include "CStaticString.hpp"
#include "Helper.hpp"

using namespace vfrb;

DESCRIBE_PAR("test_CStatisString") {
    IT("should not throw with valid initialization") {
        ASSERT_NOTHROW(CStaticString<4>("a"));
        ASSERT_NOTHROW(CStaticString<4>(String("b")));
        ASSERT_NOTHROW(CStaticString<4>(StringView("c")));
        ASSERT_NOTHROW(CStaticString<4>(CStaticString<4>("d")));
        ASSERT_NOTHROW(CStaticString<4>());
    }
    IT("should correctly copy on construction") {
        CStaticString<4> a("a");
        CStaticString<4> b(String("b"));
        CStaticString<4> c(StringView("c"));
        auto             d(CStaticString<4>("d"));
        CStaticString<4> e;
        ASSERT_EQ(*a, "a");
        ASSERT_EQ(*b, "b");
        ASSERT_EQ(*c, "c");
        ASSERT_EQ(*d, "d");
        ASSERT_EQ(*e, "");
    }
    IT("should throw if constructor overflows") {
        ASSERT_THROWS(CStaticString<4>("12345"), error::COverflowError);
        ASSERT_THROWS(CStaticString<4>(String("12345")), error::COverflowError);
        ASSERT_THROWS(CStaticString<4>(StringView("12345")), error::COverflowError);
    }

    IT("should return correct length") {
        CStaticString<4> a("abc");
        ASSERT_EQ(a.Length(), 3U);
    }
    IT("should clear correctly") {
        CStaticString<4> a("abc");
        ASSERT_EQ(a.Length(), 3U);
        a.Clear();
        ASSERT_EQ(a.Length(), 0U);
        ASSERT_EQ(*a, "");
    }
    IT("should copy on assign") {
        CStaticString<4> a;
        CStaticString<4> b;
        CStaticString<4> c;
        CStaticString<4> d;

        ASSERT_NOTHROW(a = "a");
        ASSERT_NOTHROW(b = String("b"));
        ASSERT_NOTHROW(c = StringView("c"));
        ASSERT_NOTHROW(d = CStaticString<4>("d"));
        ASSERT_EQ(*a, "a");
        ASSERT_EQ(*b, "b");
        ASSERT_EQ(*c, "c");
        ASSERT_EQ(*d, "d");
    }
    IT("should throw if assignment overflows") {
        CStaticString<4> a;
        CStaticString<4> b;
        CStaticString<4> c;

        ASSERT_THROWS(a = "12345", error::COverflowError);
        ASSERT_THROWS(b = String("12345"), error::COverflowError);
        ASSERT_THROWS(c = StringView("12345"), error::COverflowError);
    }
    IT("should correctly format string") {
        CStaticString<8> a;
        ASSERT_NOTHROW(a.Format(0, "{}", "a"));
        ASSERT_EQ(*a, StringView{"a"});
        ASSERT_EQ(a.Length(), 1U);
        ASSERT_NOTHROW(a.Format(0, "{}", 1234));
        ASSERT_EQ(*a, StringView{"1234"});
        ASSERT_EQ(a.Length(), 4U);
    }
    IT("should throw if format overflows") {
        CStaticString<4> a;
        ASSERT_THROWS(a.Format(0, "{}", "abcdefg"), error::COverflowError);
        ASSERT_THROWS(a.Format(0, "{}", "abcd"), error::COverflowError);
        ASSERT_THROWS(a.Format(2, "{}", "aaa"), error::COverflowError);
    };
};

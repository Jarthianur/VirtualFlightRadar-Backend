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

#include "String.hpp"
#include "helper.hpp"

using namespace sctf;
using namespace vfrb;

TEST_MODULE(test_String, {
    test("init", [] {
        ASSERT_NOTHROW(CString<4>("a"));
        ASSERT_NOTHROW(CString<4>(Str("b")));
        ASSERT_NOTHROW(CString<4>(StrView("c")));
        ASSERT_NOTHROW(CString<4>(CString<4>("d")));
        ASSERT_NOTHROW(CString<4>());

        CString<4> a("a");
        CString<4> b(Str("b"));
        CString<4> c(StrView("c"));
        CString<4> d(CString<4>("d"));
        CString<4> e;
        ASSERT_EQUALS(*a, {"a"});
        ASSERT_EQUALS(*b, {"b"});
        ASSERT_EQUALS(*c, {"c"});
        ASSERT_EQUALS(*d, {"d"});
        ASSERT_EQUALS(*e, {""});
    });
    test("init overflow", [] {
        ASSERT_THROWS(CString<4>("12345"), error::COverflowError);
        ASSERT_THROWS(CString<4>(Str("12345")), error::COverflowError);
        ASSERT_THROWS(CString<4>(StrView("12345")), error::COverflowError);
    });
    test("Length", [] {
        CString<4> a("abc");
        ASSERT_EQUALS(a.Length(), 3);
    });
    test("Clear", [] {
        CString<4> a("abc");
        ASSERT_EQUALS(a.Length(), 3);
        a.Clear();
        ASSERT_EQUALS(a.Length(), 0);
        ASSERT_EQUALS(*a, {""});
    });
    test("assign", [] {
        CString<4> a;
        CString<4> b;
        CString<4> c;
        CString<4> d;

        ASSERT_NOTHROW(a = "a");
        ASSERT_NOTHROW(b = Str("b"));
        ASSERT_NOTHROW(c = StrView("c"));
        ASSERT_NOTHROW(d = CString<4>("d"));
        ASSERT_EQUALS(*a, {"a"});
        ASSERT_EQUALS(*b, {"b"});
        ASSERT_EQUALS(*c, {"c"});
        ASSERT_EQUALS(*d, {"d"});
    });
    test("assign overflow", [] {
        CString<4> a;
        CString<4> b;
        CString<4> c;

        ASSERT_THROWS(a = "12345", error::COverflowError);
        ASSERT_THROWS(b = Str("12345"), error::COverflowError);
        ASSERT_THROWS(c = StrView("12345"), error::COverflowError);
    });
    test("Format", [] {
        CString<8> a;
        ASSERT_NOTHROW(a.Format(0, "%s", "a"));
        ASSERT_EQUALS(*a, {"a"});
        ASSERT_EQUALS(a.Length(), 1);
        ASSERT_NOTHROW(a.Format(0, "%d", 1234));
        ASSERT_EQUALS(*a, {"1234"});
        ASSERT_EQUALS(a.Length(), 4);
    });
    test("Format overflow", [] {
        CString<4> a;
        ASSERT_THROWS(a.Format(0, "%s", "abcdefg"), error::COverflowError);
        ASSERT_THROWS(a.Format(0, "%s", "abcd"), error::COverflowError);
        ASSERT_THROWS(a.Format(2, "%s", "aaa"), error::COverflowError);
    });
})

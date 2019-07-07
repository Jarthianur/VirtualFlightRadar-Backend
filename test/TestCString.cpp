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

#include "util/utility.hpp"

#include "helper.hpp"

using namespace sctf;
using namespace util;

TEST_MODULE(test_cstring, {
    test("initialize in range", [] {
        {
            CString<8> cstr("hello");
            assertEquals(cstr, "hello");
            assertEquals(cstr.getLength(), 5);
        }
        {
            CString<8> cstr(std::string("hello"));
            assertEquals(cstr, "hello");
            assertEquals(cstr.getLength(), 5);
        }
        {
            CString<8> t("hello");
            CString<8> cstr(t);
            assertEquals(cstr, "hello");
            assertEquals(cstr.getLength(), 5);
        }
    });
    test("assign in range", [] {
        {
            CString<8> cstr = "hello";
            assertEquals(cstr, "hello");
            assertEquals(cstr.getLength(), 5);
        }
        {
            CString<8> cstr = std::string("hello");
            assertEquals(cstr, "hello");
            assertEquals(cstr.getLength(), 5);
        }
        {
            CString<8> t("hello");
            CString<8> cstr = t;
            assertEquals(cstr, "hello");
            assertEquals(cstr.getLength(), 5);
        }
    });
    test("init/assign error", [] {
        assertException(CString<5>("hello"), std::out_of_range);
        assertException(CString<5>(std::string("hello")), std::out_of_range);
    });
})

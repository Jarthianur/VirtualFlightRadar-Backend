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

#include "server/Server.hpp"

#include "AcceptorImplTest.h"
#include "SocketImplTest.h"
#include "helper.hpp"

using namespace sctf;
using namespace vfrb;
using namespace server;

TEST_MODULE(test_Server, {
    test("connect twice", [] {
        auto                          a = std::make_shared<net::CAcceptorImplTest>(false);
        CServer<net::CSocketImplTest> s(a, 2);
        s.Run();
        usize c1 = a->Connect("#1", false, false);
        ASSERT_EQUALS(a->Socket(c1).Address(), "#1");
        a->Connect("#1", false, false);
        ASSERT(a->Sockets(), LT, 2);
    });
    test("max connections", [] {
        auto                          a = std::make_shared<net::CAcceptorImplTest>(false);
        CServer<net::CSocketImplTest> s(a, 1);
        s.Run();
        a->Connect("#1", false, false);
        ASSERT_EQUALS(a->Sockets(), 1);
        a->Connect("#2", false, false);
        ASSERT_EQUALS(a->Sockets(), 1);
    });
    test("failed connect", [] {
        auto                          a = std::make_shared<net::CAcceptorImplTest>(true);
        CServer<net::CSocketImplTest> s(a, 1);
        s.Run();
        a->Connect("#1", false, false);
        ASSERT_EQUALS(a->Sockets(), 0);
    });
    test("failed connect - error in handler", [] {
        auto                          a = std::make_shared<net::CAcceptorImplTest>(false);
        CServer<net::CSocketImplTest> s(a, 1);
        s.Run();
        a->Connect("#1", true, false);
        ASSERT_EQUALS(a->Sockets(), 0);
    });
    test("Send", [] {
        auto                          a = std::make_shared<net::CAcceptorImplTest>(false);
        CServer<net::CSocketImplTest> s(a, 2);
        s.Run();
        usize c1 = a->Connect("#1", false, false);
        usize c2 = a->Connect("#2", false, false);
        ASSERT_EQUALS(a->Sockets(), 2);
        ASSERT_EQUALS(a->Socket(c1).Address(), "#1");
        ASSERT_EQUALS(a->Socket(c2).Address(), "#2");
        s.Send("hello");
        ASSERT_EQUALS(a->Buffer(c1), "hello");
        ASSERT_EQUALS(a->Buffer(c2), "hello");
    });
    test("Send - empty msg", [] {
        auto                          a = std::make_shared<net::CAcceptorImplTest>(false);
        CServer<net::CSocketImplTest> s(a, 2);
        s.Run();
        usize c = a->Connect("#1", false, false);
        s.Send("hello");
        ASSERT_EQUALS(a->Buffer(c), "hello");
        s.Send("");
        ASSERT_EQUALS(a->Buffer(c), "hello");
    });
    test("Send - fail write", [] {
        auto                          a = std::make_shared<net::CAcceptorImplTest>(false);
        CServer<net::CSocketImplTest> s(a, 2);
        s.Run();
        usize c = a->Connect("#1", false, true);
        s.Send("hello");
        s.Send("world");
        ASSERT_EQUALS(a->Buffer(c), "");
    });
})

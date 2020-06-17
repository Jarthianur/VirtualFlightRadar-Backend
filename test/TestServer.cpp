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

#include <memory>

#include "server/CServer.hpp"

#include "CAcceptorImplTest.hpp"
#include "CSocketImplTest.hpp"
#include "Helper.hpp"
#include "Types.hpp"
#include "sctf.hpp"

using sctf::LT;

using namespace vfrb;
using namespace server;

DESCRIBE("test_CServer") {
    SPtr<net::CAcceptorImplTest>        acceptor;
    SPtr<CServer<net::CSocketImplTest>> uut;

    SETUP() {
        acceptor = std::make_shared<net::CAcceptorImplTest>();
        uut      = std::make_shared<CServer<net::CSocketImplTest>>(acceptor, 2);
    }

    BEFORE_EACH() {
        acceptor->FailOnConnect(false);
        uut->Run();
    }

    AFTER_EACH() {
        uut->Stop();
    }

    IT("should refuse more than one connection from a client at once") {
        usize c1 = acceptor->Connect("#1", false, false);
        ASSERT_EQ(acceptor->Socket(c1).Address(), "#1");
        acceptor->Connect("#1", false, false);
        ASSERT(acceptor->Sockets(), LT(), 2);
    }
    IT("should refuse any connection if maximum is reached") {
        acceptor->Connect("#1", false, false);
        ASSERT_EQ(acceptor->Sockets(), 1);
        acceptor->Connect("#2", false, false);
        ASSERT_EQ(acceptor->Sockets(), 2);
        acceptor->Connect("#3", false, false);
        ASSERT_EQ(acceptor->Sockets(), 2);
    }
    IT("should not accept client if connection fails") {
        acceptor->FailOnConnect(true);
        acceptor->Connect("#1", false, false);
        ASSERT_EQ(acceptor->Sockets(), 0);
    }
    IT("should not accept client if an error occurs in connection handler") {
        acceptor->Connect("#1", true, false);
        ASSERT_EQ(acceptor->Sockets(), 0);
    }

    IT("should send a message to clients correctly") {
        usize c1 = acceptor->Connect("#1", false, false);
        usize c2 = acceptor->Connect("#2", false, false);
        ASSERT_EQ(acceptor->Sockets(), 2);
        ASSERT_EQ(acceptor->Socket(c1).Address(), "#1");
        ASSERT_EQ(acceptor->Socket(c2).Address(), "#2");
        uut->Send("hello");
        ASSERT_EQ(acceptor->Buffer(c1), "hello");
        ASSERT_EQ(acceptor->Buffer(c2), "hello");
    }
    IT("should not send an empty message") {
        usize c = acceptor->Connect("#1", false, false);
        uut->Send("hello");
        ASSERT_EQ(acceptor->Buffer(c), "hello");
        uut->Send("");
        ASSERT_EQ(acceptor->Buffer(c), "hello");
    }
    IT("it should close connection to client if write fails") {
        usize c = acceptor->Connect("#1", false, true);
        uut->Send("hello");
        uut->Send("world");
        ASSERT_EQ(acceptor->Buffer(c), "");
    };
};

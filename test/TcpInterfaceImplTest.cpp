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

#include <thread>

#include "../src/server/Connection.hpp"
#include "SocketImplTest.h"
#include "TcpInterfaceImplTest.h"

namespace server
{
TcpInterfaceImplTests::TcpInterfaceImplTests() : TcpInterface<SocketImplTest>()
{}

TcpInterfaceImplTests::~TcpInterfaceImplTests() noexcept
{}

void TcpInterfaceImplTests::run(std::unique_lock<std::mutex>& lock)
{
    lock.unlock();
    while(!stopped)
    {
        std::this_thread::yield();
    }
}

void TcpInterfaceImplTests::stop()
{
    if(on_accept)
        on_accept(true);
}

void TcpInterfaceImplTests::onAccept(const std::function<void(bool)>& callback)
{
    on_accept = callback;
}

void TcpInterfaceImplTests::close()
{
    if(on_accept)
        on_accept(true);
}

std::unique_ptr<Connection<SocketImplTest>> TcpInterfaceImplTests::startConnection()
{
    return Connection<SocketImplTest>::create(SocketImplTest(0));
}

std::string TcpInterfaceImplTests::get_currentAddress() const
{
    return currentAddress;
}

void TcpInterfaceImplTests::connect(bool err, const std::string& adr)
{
    currentAddress = adr;
    on_accept(err);
}
}  // namespace server

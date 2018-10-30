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
#include "NetworkInterfaceImplTest.h"

namespace server
{
NetworkInterfaceImplTests::NetworkInterfaceImplTests() : NetworkInterface<SocketImplTest>()
{}

NetworkInterfaceImplTests::~NetworkInterfaceImplTests() noexcept
{}

void NetworkInterfaceImplTests::run(std::unique_lock<std::mutex>& lock)
{
    lock.unlock();
    while(!stopped)
    {
        std::this_thread::yield();
    }
}

void NetworkInterfaceImplTests::stop()
{
    if(on_accept)
        on_accept(true);
}

void NetworkInterfaceImplTests::onAccept(const std::function<void(bool)>& callback)
{
    on_accept = callback;
}

void NetworkInterfaceImplTests::close()
{
    if(on_accept)
        on_accept(true);
}

std::unique_ptr<Connection<SocketImplTest>> NetworkInterfaceImplTests::startConnection()
{
    return Connection<SocketImplTest>::create(SocketImplTest(0));
}

std::string NetworkInterfaceImplTests::get_currentAddress() const
{
    return currentAddress;
}

void NetworkInterfaceImplTests::connect(bool err, const std::string& adr)
{
    currentAddress = adr;
    on_accept(err);
}
}  // namespace server

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

#pragma once

#include "../src/server/NetworkInterface.hpp"

namespace server
{
class SocketImplTest;

class NetworkInterfaceImplTests : public NetworkInterface<SocketImplTest>
{
public:
    NetworkInterfaceImplTests();
    ~NetworkInterfaceImplTests() noexcept;

    void run(std::unique_lock<std::mutex>& lock) override;

    void stop() override;

    void onAccept(const std::function<void(bool)>& callback) override;

    void close() override;

    std::unique_ptr<Connection<SocketImplTest>> startConnection() override;

    std::string get_currentAddress() const override;

    void connect(bool err, const std::string& adr);

private:
    std::function<void(bool)> on_accept;
    std::string currentAddress;
    bool stopped = false;
};
}  // namespace server

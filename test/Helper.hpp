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

#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <boost/regex.hpp>

#include "../src/object/Aircraft.h"
#include "../src/server/Connection.hpp"
#include "../src/server/TcpInterface.hpp"
#include "framework/src/sctf.hpp"

#define TEST_FUNCTION(NAME) extern void NAME(test::TestSuitesRunner&);

#define syso(M) std::cout << M << std::endl

#define assertEqStr(V, E) assertT(V, EQUALS, E, std::string)

namespace sctf
{
namespace util
{
template<>
inline std::string
serialize<object::Aircraft::TargetType>(const object::Aircraft::TargetType& crTargetType)
{
    return std::to_string(static_cast<std::uint32_t>(crTargetType));
}
}
}

namespace helper
{
static boost::regex pflauRe("\\$PFLAU,,,,1,0,([-]?\\d+?),0,(\\d+?),(\\d+?),(\\S{6})\\*(?:\\S{2})");
static boost::regex pflaaRe(
    "\\$PFLAA,0,([-]?\\d+?),([-]?\\d+?),([-]?\\d+?),(\\d+?),(\\S{6}),(\\d{3})?,,(\\d+?)?,([-]?\\d+?\\.\\d+?)?,([0-9A-F])\\*(?:\\S{2})");
static boost::regex gpsRe(
    "(\\$GPRMC,\\d{6},A,0000\\.00,N,00000\\.00,E,0,0,\\d{6},001\\.0,W\\*[0-9a-fA-F]{2}\\s*)?(\\$GPGGA,\\d{6},0000\\.0000,N,00000\\.0000,E,1,00,1,0,M,0\\.0,M,,\\*[0-9a-fA-F]{2}\\s*)?");

}  // namespace helper

namespace object
{
namespace timestamp
{
struct DateTimeImplTest
{
    static std::int64_t now()
    {
        return 0;
    }
    static std::uint32_t day()
    {
        return 0;
    }
    static void set_day(std::uint32_t d)
    {
        _day = d;
    }
    static void set_now(std::uint32_t h, std::uint32_t m, std::uint32_t s)
    {
        _now = h * 3600000 + m * 60000 + s * 1000;
    }

private:
    static std::int64_t _now;
    static std::uint32_t _day;
};
}  // namespace timestamp
}  // namespace object

namespace server
{
class SocketImplTest
{
};

class TcpInterfaceImplTests : public TcpInterface<SocketImplTest>
{
public:
    TcpInterfaceImplTests()           = default;
    ~TcpInterfaceImplTests() noexcept = default;

    void run(std::unique_lock<std::mutex>& lock) override
    {
        while(!stopped)
        {
            std::this_thread::yield();
        }
    }

    void stop() override
    {
        if(on_accept)
            on_accept(true);
    }

    void onAccept(const std::function<void(bool)>& callback) override
    {
        on_accept = callback;
    }

    void close() override
    {
        if(on_accept)
            on_accept(true);
    }

    std::unique_ptr<Connection<SocketImplTest>> startConnection() override
    {
        return Connection<SocketImplTest>::create(SocketImplTest());
    }

    std::string get_currentAddress() const override
    {
        return currentAddress;
    }

    void connect(bool err, const std::string& adr)
    {
        currentAddress = adr;
        on_accept(err);
    }

private:
    std::function<void(bool)> on_accept;
    std::string currentAddress;
    bool stopped = false;
};
}  // namespace server

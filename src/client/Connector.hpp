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
#include <functional>
#include <string>

namespace client
{
struct Endpoint;

using Callback     = std::function<void(bool)>;
using ReadCallback = std::function<void(bool, const std::string&)>;

class Connector
{
public:
    Connector()
    {}
    virtual ~Connector() noexcept
    {}

    virtual void run()                                                          = 0;
    virtual void stop()                                                         = 0;
    virtual void close()                                                        = 0;
    virtual void onConnect(const Endpoint& endpoint, const Callback& callback)  = 0;
    virtual void onRead(const ReadCallback& crCallback)                         = 0;
    virtual void onWrite(const std::string& msg, const Callback& callback)      = 0;
    virtual void onTimeout(const Callback& callback, std::uint32_t timeout = 0) = 0;
    virtual void resetTimer(std::uint32_t timeout)                              = 0;
    virtual bool timerExpired()                                                 = 0;
};

}  // namespace client

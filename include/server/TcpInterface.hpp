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

#include <functional>
#include <memory>
#include <mutex>
#include <string>

#include "util/defines.h"

namespace server
{
template<typename SocketT>
class Connection;

template<typename SocketT>
class TcpInterface
{
public:
    TcpInterface() = default;

    virtual ~TcpInterface() noexcept = default;

    virtual void run(std::unique_lock<std::mutex>& lock)                    = 0;
    virtual void stop()                                                     = 0;
    virtual void onAccept(const std::function<void(bool)>& callback)        = 0;
    virtual void close()                                                    = 0;
    virtual std::unique_ptr<Connection<SocketT>> startConnection()          = 0;
    virtual std::string                          get_currentAddress() const = 0;
};

}  // namespace server

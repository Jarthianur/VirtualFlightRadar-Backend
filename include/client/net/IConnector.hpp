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

#pragma once

#include <functional>

#include "Types.hpp"

namespace vfrb::client::net
{
struct SEndpoint;

using Callback     = std::function<void(Result<void>&&)>;
using ReadCallback = std::function<void(Result<String>&&)>;

class IConnector
{
public:
    NOT_COPYABLE(IConnector)
    NOT_MOVABLE(IConnector)

    IConnector()                   = default;
    virtual ~IConnector() noexcept = default;

    virtual void
    Run() = 0;

    virtual void
    Stop() = 0;

    virtual void
    Close() = 0;

    virtual void
    OnConnect(SEndpoint const& ep_, Callback cb_) = 0;

    virtual void
    OnRead(ReadCallback cb_) = 0;

    virtual void
    OnWrite(String const& str_, Callback cb_) = 0;

    virtual void
    OnTimeout(Callback cb_, u32 to_) = 0;

    virtual void
    ResetTimer(u32 to_) = 0;

    virtual auto
    TimerExpired() -> bool = 0;
};
}  // namespace vfrb::client::net

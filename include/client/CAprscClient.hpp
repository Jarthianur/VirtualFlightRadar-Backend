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

#include "IClient.hpp"
#include "Types.hpp"

namespace vfrb::client
{
class CAprscClient : public IClient
{
    CTCONST KEEPALIVE_INTERVAL = 300;

    String const m_login;

    void
    sendKeepAlive() REQUIRES(m_mutex);

    void
    handleConnect(Result<void> res_) override REQUIRES(!m_mutex);

    void
    handleLogin(Result<void> res_) REQUIRES(!m_mutex);

    void
    handleSendKeepAlive(Result<void> res_) REQUIRES(!m_mutex);

    auto
    logPrefix() const -> str override;

public:
    CAprscClient(net::SEndpoint const& ep_, String const& login_, Shared<net::IConnector> con_);

    auto
    Equals(IClient const& other_) const -> bool override;
    auto
    Hash() const -> usize override;
};
}  // namespace vfrb::client

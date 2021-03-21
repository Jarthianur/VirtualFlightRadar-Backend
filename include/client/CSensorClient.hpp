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

namespace vfrb::client
{
class CSensorClient : public IClient
{
    CTCONST RECEIVE_TIMEOUT = 3;

    void
    read() override REQUIRES(m_mutex);

    void
    checkDeadline(Result<void> res_) REQUIRES(!m_mutex);

    void
    handleConnect(Result<void> res_) override REQUIRES(!m_mutex);

    auto
    logPrefix() const -> str override;

public:
    CSensorClient(net::SEndpoint const& ep_, Shared<net::IConnector> con_);
};
}  // namespace vfrb::client

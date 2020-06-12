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

#include "client/CClientManager.hpp"

#include "client/CClientFactory.hpp"
#include "concurrent/Mutex.hpp"
#include "feed/IFeed.hpp"
#include "util/StringUtils.hpp"

using vfrb::concurrent::LockGuard;

namespace vfrb::client
{
CClientManager::~CClientManager() noexcept {
    Stop();
}

void CClientManager::Subscribe(SPtr<feed::IFeed> feed_) {
    LockGuard lk(m_mutex);
    auto      it = m_clients.end();
    it           = m_clients.insert(CClientFactory::CreateClientFor(feed_)).first;
    if (it != m_clients.end()) {
        (*it)->Subscribe(feed_);
    } else {
        throw error::CFeedSubscriptionError(feed_->Name());
    }
}

void CClientManager::Run() {
    LockGuard lk(m_mutex);
    for (auto it : m_clients) {
        m_thdGroup.CreateThread([this, it] {
            it->Run();
            LockGuard lk(m_mutex);
            m_clients.erase(it);
        });
    }
}

void CClientManager::Stop() {
    LockGuard lk(m_mutex);
    for (auto it : m_clients) {
        it->ScheduleStop();
    }
}

namespace error
{
CFeedSubscriptionError::CFeedSubscriptionError(String const& name_)
    : m_msg(str_util::MakeStr("failed to subscribe ", name_, " to client")) {}

auto CFeedSubscriptionError::Message() const noexcept -> str {
    return m_msg.c_str();
}
}  // namespace error
}  // namespace vfrb::client

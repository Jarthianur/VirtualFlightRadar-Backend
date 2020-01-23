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

#include "client/ClientManager.h"

#include "client/ClientFactory.h"
#include "feed/Feed.h"
#include "util/string_utils.hpp"

using namespace vfrb::concurrent;

namespace vfrb::client
{
CClientManager::~CClientManager() noexcept
{
    Stop();
}

void CClientManager::Subscribe(SPtr<feed::IFeed> feed_)
{
    LockGuard  lk(m_mutex);
    ClientIter it = m_clients.end();
    it            = m_clients.insert(CClientFactory::CreateClientFor(feed_)).first;
    if (it != m_clients.end())
    {
        (*it)->Subscribe(feed_);
    }
    else
    {
        throw error::CFeedSubscriptionError(feed_->Name());
    }
}

void CClientManager::Run()
{
    LockGuard lk(m_mutex);
    for (auto it : m_clients)
    {
        m_thdGroup.CreateThread([this, it] {
            it->Run();
            LockGuard lk(m_mutex);
            m_clients.erase(it);
        });
    }
}

void CClientManager::Stop()
{
    LockGuard lk(m_mutex);
    for (auto it : m_clients)
    {
        it->ScheduleStop();
    }
}

namespace error
{
CFeedSubscriptionError::CFeedSubscriptionError(Str const& name_)
    : m_msg(str_util::MakeStr("failed to subscribe ", name_, " to client"))
{}

char const* CFeedSubscriptionError::Message() const noexcept
{
    return m_msg.c_str();
}
}  // namespace error
}  // namespace vfrb::client

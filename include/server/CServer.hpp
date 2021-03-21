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

#include <algorithm>

#include "concurrent/CGuardedThread.hpp"
#include "concurrent/Mutex.hpp"
#include "net/impl/CAcceptorAsio.hpp"

#include "CConnection.hpp"
#include "CLogger.hpp"
#include "Types.hpp"

namespace vfrb::server
{
template<typename SocketT>
class CServer
{
    NOT_COPYABLE(CServer)
    NOT_MOVABLE(CServer)

    str const LOG_PREFIX = "(Server) ";

    concurrent::Mutex mutable m_mutex;
    Shared<net::IAcceptor<SocketT>> GUARDED_BY(m_mutex) m_acceptor;
    Vector<CConnection<SocketT>>    GUARDED_BY(m_mutex) m_connections;
    usize const                     GUARDED_BY(m_mutex) m_maxConnections;
    bool                            GUARDED_BY(m_mutex) m_running = false;
    concurrent::CGuardedThread      GUARDED_BY(m_mutex) m_thread;

    void
    accept() REQUIRES(!m_mutex);

    auto
    isConnected(String const& addr_) -> bool REQUIRES(m_mutex);

    void
    handleStagedConnection(Result<void> res_) noexcept REQUIRES(!m_mutex);

public:
    CServer(u16 port_, usize maxCon_);
    CServer(Shared<net::IAcceptor<SocketT>> acceptor_, usize maxCon_);
    ~CServer() noexcept;

    void
    Run() REQUIRES(!m_mutex);
    void
    Stop() REQUIRES(!m_mutex);

    void
    Send(String const& str_) REQUIRES(!m_mutex);
};

template<typename SocketT>
CServer<SocketT>::CServer(u16 port_, usize maxCon_)
    : CServer(AllocShared<net::CAcceptorAsio>(port_), maxCon_) {}

template<typename SocketT>
CServer<SocketT>::CServer(Shared<net::IAcceptor<SocketT>> acceptor_, usize maxCon_)
    : m_acceptor(acceptor_), m_maxConnections(maxCon_) {
    m_connections.reserve(maxCon_);
}

template<typename SocketT>
CServer<SocketT>::~CServer() noexcept {
    Stop();
}

template<typename SocketT>
void
CServer<SocketT>::Run() REQUIRES(!m_mutex) {
    concurrent::ImmutableLock lk(m_mutex);
    CLogger::Instance().Info(LOG_PREFIX, "starting...");
    m_running = true;
    m_thread.Spawn([this]() {
        accept();
        m_acceptor->Run();
        CLogger::Instance().Debug(LOG_PREFIX, "stopped");
    });
}

template<typename SocketT>
void
CServer<SocketT>::Stop() REQUIRES(!m_mutex) {
    concurrent::ImmutableLock lk(m_mutex);
    if (m_running) {
        m_running = false;
        CLogger::Instance().Info(LOG_PREFIX, "stopping all connections...");
        m_connections.clear();
        m_acceptor->Stop();
    }
}

template<typename SocketT>
void
CServer<SocketT>::Send(String const& str_) REQUIRES(!m_mutex) {
    concurrent::ImmutableLock lk(m_mutex);
    if (str_.empty() || m_connections.empty()) {
        return;
    }
    for (auto it = m_connections.begin(); it != m_connections.end();) {
        if (!it->Write(str_)) {
            CLogger::Instance().Warn(LOG_PREFIX, "lost connection to: ", it->Address());
            it = m_connections.erase(it);
        } else {
            ++it;
        }
    }
}

template<typename SocketT>
void
CServer<SocketT>::accept() REQUIRES(!m_mutex) {
    concurrent::ImmutableLock lk(m_mutex);
    m_acceptor->OnAccept([this](Result<void>&& res_) { handleStagedConnection(res_); });
}

template<typename SocketT>
auto
CServer<SocketT>::isConnected(String const& addr_) -> bool REQUIRES(m_mutex) {
    return std::find_if(m_connections.begin(), m_connections.end(),
                        [&](auto const& it_) { return it_.Address() == addr_; }) != m_connections.end();
}

template<typename SocketT>
void
CServer<SocketT>::handleStagedConnection(Result<void> res_) noexcept REQUIRES(!m_mutex) {
    if (res_) {
        concurrent::ImmutableLock lk(m_mutex);
        try {
            if (m_connections.size() < m_maxConnections && !isConnected(m_acceptor->StagedAddress())) {
                m_connections.push_back(m_acceptor->StartConnection());
                CLogger::Instance().Info(LOG_PREFIX, "connection from: ", m_connections.crbegin()->Address());
            } else {
                CLogger::Instance().Info(LOG_PREFIX, "refused connection to ", m_acceptor->StagedAddress());
                m_acceptor->Close();
            }
        } catch (net::error::CSocketError const& e) {
            CLogger::Instance().Warn(LOG_PREFIX, "connection failed: ", e.Message());
            m_acceptor->Close();
        }
    } else {
        CLogger::Instance().Warn(LOG_PREFIX, "could not accept connection");
        m_acceptor->Close();
    }
    accept();
}
}  // namespace vfrb::server

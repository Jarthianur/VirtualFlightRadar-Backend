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
#include <vector>

#include "concurrent/CGuardedThread.hpp"
#include "concurrent/Mutex.hpp"
#include "net/impl/CAcceptorBoost.hpp"
#include "util/ClassUtils.hpp"

#include "CConnection.hpp"
#include "CLogger.hpp"
#include "Types.hpp"

namespace vfrb::server
{
/**
 * A TCP server to serve single messages to all clients.
 * @tparam SocketT The socket implementation
 */
template<typename SocketT>
class CServer
{
    NOT_COPYABLE(CServer)
    NOT_MOVABLE(CServer)

    static str const      LOG_PREFIX;
    static CLogger const& s_logger;

    concurrent::Mutex mutable m_mutex;
    SPtr<net::IAcceptor<SocketT>>     GUARDED_BY(m_mutex) m_acceptor;     ///< Acceptor interface
    std::vector<CConnection<SocketT>> GUARDED_BY(m_mutex) m_connections;  ///< Connections container
    usize const                GUARDED_BY(m_mutex) m_maxConnections;   ///< Max amount of active connections
    bool                       GUARDED_BY(m_mutex) m_running = false;  ///< Run state
    concurrent::CGuardedThread GUARDED_BY(m_mutex) m_thread;           ///< Internal thread

    void
    accept() REQUIRES(!m_mutex);

    /**
     * Check whether an ip address already exists in the connection container.
     * @param addr_ The ip address to check
     * @return true if the ip is already registered, else false
     */
    auto
    isConnected(String const& addr_) -> bool REQUIRES(m_mutex);

    /**
     * Handler for accepting incoming connections.
     * @param err_ The error indicator, if false connection can not be accepted
     */
    void
    handleStagedConnection(bool err_) noexcept REQUIRES(!m_mutex);

public:
    CServer(u16 port_, usize maxCon_);
    CServer(SPtr<net::IAcceptor<SocketT>> acceptor_, usize maxCon_);
    ~CServer() noexcept;

    void
    Run() REQUIRES(!m_mutex);
    void
    Stop() REQUIRES(!m_mutex);

    /**
     * Send a message to all clients.
     * @param sv_ The message
     */
    void
    Send(String const& str_) REQUIRES(!m_mutex);
};

template<typename SocketT>
str const CServer<SocketT>::LOG_PREFIX = "(Server) ";

template<typename SocketT>
CLogger const& CServer<SocketT>::s_logger = CLogger::Instance();

template<typename SocketT>
CServer<SocketT>::CServer(u16 port_, usize maxCon_)
    : CServer(std::make_shared<net::CAcceptorBoost>(port_), maxCon_) {}

template<typename SocketT>
CServer<SocketT>::CServer(SPtr<net::IAcceptor<SocketT>> acceptor_, usize maxCon_)
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
    concurrent::LockGuard lk(m_mutex);
    s_logger.Info(LOG_PREFIX, "starting...");
    m_running = true;
    m_thread.Spawn([this]() {
        accept();
        m_acceptor->Run();
        s_logger.Debug(LOG_PREFIX, "stopped");
    });
}

template<typename SocketT>
void
CServer<SocketT>::Stop() REQUIRES(!m_mutex) {
    concurrent::LockGuard lk(m_mutex);
    if (m_running) {
        m_running = false;
        s_logger.Info(LOG_PREFIX, "stopping all connections...");
        m_connections.clear();
        m_acceptor->Stop();
    }
}

template<typename SocketT>
void
CServer<SocketT>::Send(String const& str_) REQUIRES(!m_mutex) {
    concurrent::LockGuard lk(m_mutex);
    if (str_.empty() || m_connections.empty()) {
        return;
    }
    for (auto it = m_connections.begin(); it != m_connections.end();) {
        if (!it->Write(str_)) {
            s_logger.Warn(LOG_PREFIX, "lost connection to: ", it->Address());
            it = m_connections.erase(it);
        } else {
            ++it;
        }
    }
}

template<typename SocketT>
void
CServer<SocketT>::accept() REQUIRES(!m_mutex) {
    concurrent::LockGuard lk(m_mutex);
    m_acceptor->OnAccept([this](bool err_) { handleStagedConnection(err_); });
}

template<typename SocketT>
auto
CServer<SocketT>::isConnected(String const& addr_) -> bool REQUIRES(m_mutex) {
    return std::find_if(m_connections.begin(), m_connections.end(),
                        [&](auto const& it_) { return it_.Address() == addr_; }) != m_connections.end();
}

template<typename SocketT>
void
CServer<SocketT>::handleStagedConnection(bool err_) noexcept REQUIRES(!m_mutex) {
    if (!err_) {
        concurrent::LockGuard lk(m_mutex);
        try {
            if (m_connections.size() < m_maxConnections && !isConnected(m_acceptor->StagedAddress())) {
                m_connections.push_back(m_acceptor->StartConnection());
                s_logger.Info(LOG_PREFIX, "connection from: ", m_connections.crbegin()->Address());
            } else {
                s_logger.Info(LOG_PREFIX, "refused connection to ", m_acceptor->StagedAddress());
                m_acceptor->Close();
            }
        } catch (net::error::CSocketError const& e) {
            s_logger.Warn(LOG_PREFIX, "connection failed: ", e.Message());
            m_acceptor->Close();
        }
    } else {
        s_logger.Warn(LOG_PREFIX, "could not accept connection");
        m_acceptor->Close();
    }
    accept();
}
}  // namespace vfrb::server

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

#include <algorithm>
#include <string_view>
#include <vector>

#include "concurrent/GuardedThread.hpp"
#include "concurrent/Mutex.hpp"
#include "net/impl/NetworkInterfaceImplBoost.h"
#include "util/Logger.hpp"
#include "util/class_utils.h"

#include "Connection.hpp"
#include "types.h"

namespace vfrb::server
{
/**
 * @brief A TCP server to serve the same reports to all clients.
 * @tparam SocketT The socket implementation
 */
template<typename SocketT>
class CServer
{
    NOT_COPYABLE(CServer)

    static char const*    LOG_PREFIX;
    static CLogger const& s_logger;

    concurrent::Mutex mutable m_mutex;
    SPtr<net::IAcceptor<SocketT>>     GUARDED_BY(m_mutex) m_acceptor;     ///< NetworkInterface
    std::vector<CConnection<SocketT>> GUARDED_BY(m_mutex) m_connections;  ///< Connections container
    usize const                       GUARDED_BY(m_mutex) m_maxConnections;
    bool                              GUARDED_BY(m_mutex) m_running = false;  ///< Running state
    concurrent::CGuardedThread        GUARDED_BY(m_mutex) m_thread;           ///< Internal thread

    /**
     * @brief Schedule to accept connections.
     */
    void accept() REQUIRES(!m_mutex);

    /**
     * @brief Check whether an ip address already exists in the Connection container.
     * @param address The ip address to check
     * @return true if the ip is already registered, else false
     */
    bool isConnected(Str const& address_) REQUIRES(m_mutex);

    /**
     * @brief Handler for accepting connections.
     * @param error The error indicator
     */
    void attemptConnection(bool error_) noexcept REQUIRES(!m_mutex);

public:
    CServer(u16 port_, usize maxCon_);  ///< @param port The port
    CServer(SPtr<net::IAcceptor<SocketT>> interface_,
            usize                         maxCon_);  ///< @param interface The NetworkInterface to use
    ~CServer() noexcept;

    /**
     * @brief Run the Server.
     * @threadsafe
     */
    void Run() REQUIRES(!m_mutex);

    /**
     * @brief Stop all connections.
     * @threadsafe
     */
    void Stop() REQUIRES(!m_mutex);

    /**
     * @brief Write a message to all clients.
     * @param msg The msg to write
     * @threadsafe
     */
    void Send(std::string_view const& msg_) REQUIRES(!m_mutex);
};

template<typename SocketT>
char const* CServer<SocketT>::LOG_PREFIX = "(Server) ";

template<typename SocketT>
CLogger const& CServer<SocketT>::s_logger = CLogger::Instance();

template<typename SocketT>
CServer<SocketT>::CServer(u16 port_, usize maxCon_)
    : CServer(std::make_shared<net::CAcceptorBoost>(port_), maxCon_)
{}

template<typename SocketT>
CServer<SocketT>::CServer(SPtr<net::IAcceptor<SocketT>> interface_, usize maxCon_)
    : m_acceptor(interface_), m_maxConnections(maxCon_)
{
    m_connections.reserve(maxCon_);
}

template<typename SocketT>
CServer<SocketT>::~CServer() noexcept
{
    Stop();
}

template<typename SocketT>
void CServer<SocketT>::Run() REQUIRES(!m_mutex)
{
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
void CServer<SocketT>::Stop() REQUIRES(!m_mutex)
{
    concurrent::LockGuard lk(m_mutex);
    if (m_running)
    {
        m_running = false;
        s_logger.Info(LOG_PREFIX, "stopping all connections...");
        m_connections.clear();
        m_acceptor->Stop();
    }
}

template<typename SocketT>
void CServer<SocketT>::Send(std::string_view const& msg_) REQUIRES(!m_mutex)
{
    concurrent::LockGuard lk(m_mutex);
    if (msg_.empty() || m_connections.empty())
    {
        return;
    }
    for (auto it = m_connections.begin(); it != m_connections.end();)
    {
        if (!it->Write(msg_))
        {
            s_logger.Warn(LOG_PREFIX, "lost connection to: ", it->Address());
            it = m_connections.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

template<typename SocketT>
void CServer<SocketT>::accept() REQUIRES(!m_mutex)
{
    concurrent::LockGuard lk(m_mutex);
    m_acceptor->OnAccept(std::bind(&CServer<SocketT>::attemptConnection, this, std::placeholders::_1));
}

template<typename SocketT>
bool CServer<SocketT>::isConnected(Str const& address_) REQUIRES(m_mutex)
{
    return std::find_if(m_connections.begin(), m_connections.end(),
                        [&](auto const& it_) { return it_.Address() == address_; }) != m_connections.end();
}

template<typename SocketT>
void CServer<SocketT>::attemptConnection(bool error_) noexcept REQUIRES(!m_mutex)
{
    if (!error_)
    {
        concurrent::LockGuard lk(m_mutex);
        try
        {
            if (m_connections.size() < m_maxConnections && !isConnected(m_acceptor->StagedAddress()))
            {
                m_connections.push_back(m_acceptor->StartConnection());
                s_logger.Info(LOG_PREFIX, "connection from: ", m_connections.crbegin()->Address());
            }
            else
            {
                s_logger.Info(LOG_PREFIX, "refused connection to ", m_acceptor->StagedAddress());
                m_acceptor->Close();
            }
        }
        catch (net::error::CSocketError const& e)
        {
            s_logger.Warn(LOG_PREFIX, "connection failed: ", e.What());
            m_acceptor->Close();
        }
    }
    else
    {
        s_logger.Warn(LOG_PREFIX, "could not accept connection");
    }
    accept();
}
}  // namespace vfrb::server

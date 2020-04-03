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

#include <utility>

#include "net/error/CSocketError.hpp"
#include "util/class_utils.hpp"

#include "CLogger.hpp"
#include "types.hpp"

namespace vfrb::server
{
/**
 * @brief TCP connection opened by the Server.
 * @tparam SocketT The type of socket implementation
 */
template<typename SocketT>
class CConnection
{
    SocketT m_socket;   ///< Socket
    String  m_address;  ///< IP address

public:
    MOVABLE_BUT_NOT_COPYABLE(CConnection)

    explicit CConnection(SocketT&& sock_);

    ~CConnection() noexcept = default;

    /**
     * Write a message to the endpoint.
     * @param sv_ The message
     * @return true on success, else false
     */
    bool Write(StringView const& sv_);

    auto Address() const -> decltype(m_address) const&;
};

template<typename SocketT>
CConnection<SocketT>::CConnection(SocketT&& sock_)
    : m_socket(std::move(sock_)), m_address(m_socket.Address()) {}

template<typename SocketT>
CConnection<SocketT>::CConnection(CConnection&& other_) noexcept
    : m_socket(std::move(other_.m_socket)), m_address(std::move(other_.m_address)) {}

template<typename SocketT>
CConnection<SocketT>& CConnection<SocketT>::operator=(CConnection&& other_) noexcept {
    m_socket  = std::move(other_.m_socket);
    m_address = std::move(other_.m_address);
    return *this;
}

template<typename SocketT>
bool CConnection<SocketT>::Write(StringView const& sv_) {
    try {
        return m_socket.Write(sv_);
    } catch (net::error::CSocketError const& e) {
        CLogger::Instance().Debug("(Connection) write: ", e.Message());
    }
    return false;
}

template<typename SocketT>
auto CConnection<SocketT>::Address() const -> decltype(m_address) const& {
    return m_address;
}
}  // namespace vfrb::server

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

#include <istream>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include "client/net/IConnector.hpp"

namespace vfrb::client::net
{
/**
 * @brief Implement the Connector interface using boost::asio.
 */
class CConnectorBoost : public IConnector
{
    boost::asio::io_context        m_ioCtx;     ///< Internal IO-service
    boost::asio::ip::tcp::socket   m_socket;    ///< Connection socket
    boost::asio::ip::tcp::resolver m_resolver;  ///< Host resolver
    boost::asio::deadline_timer    m_timer;     ///< Timer
    boost::asio::streambuf         m_buffer;    ///< Read buffer
    String                         m_response;  ///< Read message
    std::istream                   m_istream;   ///< Message stream for conversion

    EErrc evalErrorCode(boost::system::error_code const& err_) const;

    /**
     * @brief Handler for resolving endpoint
     * @param error      The error code
     * @param resolverIt The resolved host
     * @param callback   The callback to invoke
     */
    void handleResolve(boost::system::error_code const&             err_,
                       boost::asio::ip::tcp::resolver::results_type res_, Callback const& cb_) noexcept;

    /**
     * @brief Handler for connecting to endpoint
     * @param error      The error code
     * @param resolverIt The resolved host
     * @param callback   The callback to invoke
     */
    void handleConnect(boost::system::error_code const& err_, Callback const& cb_) noexcept;

    /**
     * @brief Handler for timed out timer
     * @param error    The error code
     * @param callback The callback to invoke
     */
    void handleTimeout(boost::system::error_code const& err_, Callback const& cb_) noexcept;

    /**
     * @brief Handler for reading from endpoint
     * @param error    The error code
     * @param bytes    The amount of read bytes
     * @param callback The callback to invoke
     */
    void handleRead(boost::system::error_code const& err_, usize bytes_, ReadCallback const& cb_) noexcept;

    /**
     * @brief Handler for writing to endpoint
     * @param error    The error code
     * @param bytes    The amount of sent bytes
     * @param callback The callback to invoke
     */
    void handleWrite(boost::system::error_code const& err_, usize bytes_, Callback const& cb_) noexcept;

public:
    CConnectorBoost();

    /**
     * @brief Run the internal event handler queue.
     * @note Blocks until all handlers have returned.
     */
    void Run() override;

    /**
     * @brief Stop the internal event handler queue and close the connection.
     */
    void Stop() override;

    /**
     * @brief Close the connection and cancel all timers.
     */
    void Close() override;

    /**
     * @brief Schedule to connect.
     * @param endpoint The endpoint to connect to
     * @param callback The callback to invoke when done
     */
    void OnConnect(SEndpoint const& ep_, Callback const& cb_) override;

    /**
     * @brief Schedule to read from endpoint.
     * @param callback The callback to invoke when done
     */
    void OnRead(ReadCallback const& cb_) override;

    /**
     * @brief Schedule to write to endpoint.
     * @param msg      The message to send
     * @param callback The callback to invoke when done
     */
    void OnWrite(String const& str_, Callback const& cb_) override;

    /**
     * @brief Schedule an action after a timeout.
     * @note A timeout of 0 only adds the action and does not modify the timer.
     * @param callback The callback to invoke when timeout is reached
     * @param timeout  The timeout in seconds (default: 0)
     */
    void OnTimeout(Callback const& cb_, u32 to_ = 0) override;

    /**
     * @brief Reset the timer to this value.
     * @param timeout The new timeout
     */
    void ResetTimer(u32 to_) override;

    /**
     * @brief Check whether the timer is expired.
     * @return true if expired, else false
     */
    bool TimerExpired() override;
};
}  // namespace vfrb::client::net

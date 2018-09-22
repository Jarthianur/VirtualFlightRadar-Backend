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

#include <stdexcept>
#include "Client.hpp"

/// @namespace client
namespace client
{
/**
 * @class AprscClient
 * @brief Connect to an APRSC server.
 * @extends Client
 */
template<typename ConnectorT>
class AprscClient : public Client<ConnectorT>
{
public:
    NOT_COPYABLE(AprscClient)

    /**
     * @fn AprscClient
     * @brief Constructor
     * @param crHost  The hostname
     * @param crPort  The port
     * @param crLogin The login string to transmit
     * @param rFeed   The handler Feed reference
     */
    AprscClient(const Endpoint& endpoint, const std::string& login);

    /**
     * @fn ~AprscClient
     * @brief Destructor
     */
    ~AprscClient() noexcept;

    bool equals(const Client<ConnectorT>& other) const override;

    std::size_t hash() const override;

private:
    /**
     * @fn sendKeepAlive
     * @brief Send a keep-alive beacon every 10 minutes.
     */
    void sendKeepAlive();

    /**
     * @see Client#handleConnect
     */
    void handleConnect(bool error) noexcept override;

    /**
     * @fn handleLogin
     * @brief Handler login string sending.
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleLogin(bool error) noexcept;

    /**
     * @fn handleSendKeepAlive
     * @brief Handler for sendKeepAlive
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleSendKeepAlive(bool error) noexcept;

    /// @var mLoginStr
    /// Login string
    const std::string m_login;
};

template<typename ConnectorT>
AprscClient<ConnectorT>::AprscClient(const Endpoint& endpoint, const std::string& login)
    : Client<ConnectorT>(endpoint, "(AprscClient)"), m_login(login + "\r\n")
{}

template<typename ConnectorT>
AprscClient<ConnectorT>::~AprscClient() noexcept
{}

template<typename ConnectorT>
bool AprscClient<ConnectorT>::equals(const Client<ConnectorT>& other) const
{
    try
    {
        const AprscClient& derived = dynamic_cast<const AprscClient&>(other);
        return Client<ConnectorT>::equals(other) && this->m_login == derived.m_login;
    }
    catch(const std::bad_cast&)
    {
        return false;
    }
}

template<typename ConnectorT>
std::size_t AprscClient<ConnectorT>::hash() const
{
    std::size_t seed = Client<ConnectorT>::hash();
    boost::hash_combine(seed, boost::hash_value(m_login));
    return seed;
}

template<typename ConnectorT>
void AprscClient<ConnectorT>::handleConnect(bool error) noexcept
{
    if(!error)
    {
        std::lock_guard<std::mutex> lock(this->m_mutex);
        this->m_connector.onWrite(
            m_login, std::bind(&AprscClient::handleLogin, this, std::placeholders::_1));
        sendKeepAlive();
    }
    else
    {
        logger.warn(this->m_component, " failed to connect to ", this->m_endpoint.host, ":",
                    this->m_endpoint.port);
        this->reconnect();
    }
}

template<typename ConnectorT>
void AprscClient<ConnectorT>::sendKeepAlive()
{
    this->m_connector.onTimeout(
        std::bind(&AprscClient::handleSendKeepAlive, this, std::placeholders::_1), 600);
}

template<typename ConnectorT>
void AprscClient<ConnectorT>::handleLogin(bool error) noexcept
{
    if(!error)
    {
        logger.info(this->m_component, " connected to ", this->m_endpoint.host, ":",
                    this->m_endpoint.port);
        std::lock_guard<std::mutex> lock(this->m_mutex);
        this->read();
    }
    else
    {
        logger.error(this->m_component, " send login failed");
    }
}

template<typename ConnectorT>
void AprscClient<ConnectorT>::handleSendKeepAlive(bool error) noexcept
{
    if(!error)
    {
        std::lock_guard<std::mutex> lock(this->m_mutex);
        this->m_connector.onWrite("#keep-alive beacon\r\n", [this](bool error) {
            if(!error)
            {
                sendKeepAlive();
            }
            else
            {
                logger.error(this->m_component, " send keep-alive beacon failed");
                this->reconnect();
            }
        });
    }
}

}  // namespace client

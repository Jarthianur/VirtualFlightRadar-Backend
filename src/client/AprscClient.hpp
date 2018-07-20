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
    AprscClient(const Endpoint& crEndpoint, const std::string& crLogin);

    /**
     * @fn ~AprscClient
     * @brief Destructor
     */
    ~AprscClient() noexcept;

    bool equals(const Client<ConnectorT>& crOther) const override;

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
    void handleConnect(bool vError) noexcept override;

    /**
     * @fn handleLogin
     * @brief Handler login string sending.
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleLogin(bool vError) noexcept;

    /**
     * @fn handleSendKeepAlive
     * @brief Handler for sendKeepAlive
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleSendKeepAlive(bool vError) noexcept;

    /// @var mLoginStr
    /// Login string
    std::string mLoginStr;
};

template<typename ConnectorT>
AprscClient<ConnectorT>::AprscClient(const Endpoint& crEndpoint, const std::string& crLogin)
    : Client<ConnectorT>(crEndpoint, "(AprscClient)"), mLoginStr(crLogin)
{
    mLoginStr.append("\r\n");
}

template<typename ConnectorT>
AprscClient<ConnectorT>::~AprscClient() noexcept
{}

template<typename ConnectorT>
bool AprscClient<ConnectorT>::equals(const Client<ConnectorT>& crOther) const
{
    try
    {
        const AprscClient& crAOther = dynamic_cast<const AprscClient&>(crOther);
        return Client<ConnectorT>::equals(crOther) && this->mLoginStr == crAOther.mLoginStr;
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
    boost::hash_combine(seed, boost::hash_value(mLoginStr));
    return seed;
}

template<typename ConnectorT>
void AprscClient<ConnectorT>::handleConnect(bool vError) noexcept
{
    if(vError)
    {
        std::lock_guard<std::mutex> lock(this->mMutex);
        this->mConnector.onWrite(mLoginStr,
                                 std::bind(&AprscClient::handleLogin, this, std::placeholders::_1));
        sendKeepAlive();
    }
    else
    {
        logger.warn(this->mComponent, " failed to connect to ", this->mEndpoint.host, ":",
                    this->mEndpoint.port);
        this->reconnect();
    }
}

template<typename ConnectorT>
void AprscClient<ConnectorT>::sendKeepAlive()
{
    this->mConnector.onTimeout(
        std::bind(&AprscClient::handleSendKeepAlive, this, std::placeholders::_1), 600);
}

template<typename ConnectorT>
void AprscClient<ConnectorT>::handleLogin(bool vError) noexcept
{
    if(vError)
    {
        logger.info(this->mComponent, " connected to ", this->mEndpoint.host, ":",
                    this->mEndpoint.port);
        std::lock_guard<std::mutex> lock(this->mMutex);
        this->read();
    }
    else
    {
        logger.error(this->mComponent, " send login failed");
    }
}

template<typename ConnectorT>
void AprscClient<ConnectorT>::handleSendKeepAlive(bool vError) noexcept
{
    if(vError)
    {
        std::lock_guard<std::mutex> lock(this->mMutex);
        this->mConnector.onWrite("#keep-alive beacon\r\n", [this](bool vError) {
            if(!vError)
            {
                logger.error(this->mComponent, " send keep-alive beacon failed");
                this->reconnect();
            }
        });
        sendKeepAlive();
    }
}

}  // namespace client

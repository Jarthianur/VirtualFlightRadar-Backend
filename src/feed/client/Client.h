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

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <boost/date_time.hpp>
#include <boost/functional/hash.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

#include "../../Defines.h"
#include "../../Logger.hpp"
#include "../Feed.h"

/// @namespace feed
namespace feed
{
/// @namespace client
namespace client
{
#ifdef CLIENT_CONNECT_WAIT_TIMEVAL
/// @def C_CON_WAIT_TIMEVAL
/// Wait for (re-)connect timeout
#define C_CON_WAIT_TIMEVAL CLIENT_CONNECT_WAIT_TIMEVAL
#else
#define C_CON_WAIT_TIMEVAL 120
#endif

struct Endpoint
{
    const std::string host;
    const std::string port;

    bool operator==(const Endpoint& crOther) const
    {
        return host == crOther.host && port == crOther.port;
    }
};

/**
 * @class Client
 * @brief Base class representing a TCP client.
 * @note A Client is unique and only movable.
 */
template<typename ConnectorT>
class Client
{
public:
    NOT_COPYABLE(Client)

    /**
     * @fn ~Client
     * @brief Destructor
     */
    virtual ~Client() noexcept;

    /**
     * @fn run
     * @brief Run the Client.
     * @note Returns after all queued handlers have returned.
     * @param rSigset The signal set reference to register handler
     */
    void run();

    void lockAndStop();

    virtual bool equals(const Client<ConnectorT>& crOther) const;

    virtual std::size_t hash() const;

    void subscribe(std::shared_ptr<Feed>& rpFeed);

protected:
    /**
     * @fn Client
     * @brief Constructor
     * @param crHost       The hostname
     * @param crPort       The port
     * @param crComponent  The component name
     * @param rFeed        The handler Feed reference
     */
    Client(const Endpoint& crEndpoint, const std::string& crComponent);

    /**
     * @fn stop
     * @brief Stop the Client and close the connection.
     */
    virtual void stop();

    /**
     * @fn timedConnect
     * @brief Connect after timeout.
     */
    void timedConnect();

    /**
     * @fn read
     * @brief Read from remote endpoint.
     */
    virtual void read();

    /**
     * @fn connect
     * @brief Connect to host.
     */
    virtual void connect() = 0;

    /**
     * @fn handleTimedConnect
     * @brief Handler for timedConnect.
     * @param crError The error code
     */
    void handleTimedConnect(bool vError) noexcept;

    /**
     * @fn handleRead
     * @brief Handler for read.
     * @param crError The error code
     * @param vBytes  The sent bytes
     */
    void handleRead(bool vError, const std::string& crResponse) noexcept;

    /**
     * @fn handleResolve
     * @brief Handler for resolve host.
     * @param crError     The error code
     * @param vResloverIt The resolve iterator
     */
    virtual void handleResolve(const boost::system::error_code& crError,
                               boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept
        = 0;

    /**
     * @fn handleConnect
     * @brief Handler for connect.
     * @param crError     The error code
     * @param vResolverIt The resolve iterator
     */
    virtual void handleConnect(const boost::system::error_code& crError,
                               boost::asio::ip::tcp::resolver::iterator vResolverIt) noexcept
        = 0;

    ConnectorT mConnector;

    boost::mutex mMutex;

    bool mRunning = false;

    /// @var mComponent
    /// Component string used for logging
    const std::string mComponent;

    /// @var mHost
    /// Hostname
    Endpoint mEndpoint;

private:
    /// @var mrFeeds
    /// Handler Feed references
    std::vector<std::shared_ptr<feed::Feed>> mrFeeds;
};

template<typename ConnectorT>
Client<ConnectorT>::Client(const Endpoint& crEndpoint, const std::string& crComponent)
    : mEndpoint(crEndpoint), mComponent(crComponent)
{}

template<typename ConnectorT>
Client<ConnectorT>::~Client() noexcept
{}

template<typename ConnectorT>
void Client<ConnectorT>::run()
{
    if(!mRunning)
    {
        {
            boost::lock_guard<boost::mutex> lock(mMutex);
            connect();
        }
        mConnector.run();
    }
}

template<typename ConnectorT>
bool Client<ConnectorT>::equals(const Client& crOther) const
{
    return this->mEndpoint == crOther.mEndpoint;
}

template<typename ConnectorT>
std::size_t Client<ConnectorT>::hash() const
{
    std::size_t seed = 0;
    boost::hash_combine(seed, boost::hash_value(mEndpoint.host));
    boost::hash_combine(seed, boost::hash_value(mEndpoint.port));
    return seed;
}

template<typename ConnectorT>
void Client<ConnectorT>::subscribe(std::shared_ptr<Feed>& rpFeed)
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    mrFeeds.push_back(rpFeed);
}

template<typename ConnectorT>
void Client<ConnectorT>::timedConnect()
{
    mConnector.onTimeout(C_CON_WAIT_TIMEVAL,
                         std::bind(&Client::handleTimedConnect, this, std::placeholders::_1));
}

template<typename ConnectorT>
void Client<ConnectorT>::stop()
{
    if(mRunning)
    {
        mRunning = false;
        logger.info(mComponent, " disconnect from ", mEndpoint.host, ":", mEndpoint.port);
        mConnector.stop();
    }
}

template<typename ConnectorT>
void Client<ConnectorT>::lockAndStop()
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    stop();
}

template<typename ConnectorT>
void Client<ConnectorT>::read()
{
    if(mRunning)
    {
        mConnector.onRead(std::bind(&Client::handleRead, this, std::placeholders::_1));
    }
}

template<typename ConnectorT>
void Client<ConnectorT>::handleTimedConnect(bool vError) noexcept
{
    if(vError)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        logger.info(mComponent, " try connect to ", mEndpoint.host, ":", mEndpoint.port);
        connect();
    }
    else
    {
        logger.error(mComponent, " failed to connect after timeout");
        lockAndStop();
    }
}

template<typename ConnectorT>
void Client<ConnectorT>::handleRead(bool vError, const std::string& crResponse) noexcept
{
    if(vError)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        for(auto& it : mrFeeds)
        {
            if(!mRunning)
            {
                break;
            }
            it->process(crResponse);
        }
        read();
    }
    else
    {
        logger.error(mComponent, " failed to read message");
        boost::lock_guard<boost::mutex> lock(mMutex);
        mConnector.close();
        timedConnect();
    }
}

}  // namespace client
}  // namespace feed

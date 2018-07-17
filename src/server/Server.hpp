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

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <boost/thread.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

#include "../Logger.hpp"

#include "../Defines.h"
#include "../Parameters.h"
#include "Connection.hpp"

#ifdef SERVER_MAX_CLIENTS
/// @def S_MAX_CLIENTS
/// The max amount of client to accept at once
#define S_MAX_CLIENTS SERVER_MAX_CLIENTS
#else
#define S_MAX_CLIENTS 5
#endif

/// @namespace server
namespace server
{
/**
 * @class Server
 * @brief A TCP server to serve the same reports to all clients.
 */
template<typename TcpInterfaceT, typename SocketT>
class Server
{
public:
    NOT_COPYABLE(Server)

    Server() : Server(4353)
    {}

    /**
     * @fn Server
     * @brief Constructor
     * @param vPort The port
     */
    explicit Server(std::uint16_t vPort) : mTcpIf(vPort)
    {}

    ~Server() noexcept
    {}

    /**
     * @fn run
     * @brief Run the Server.
     * @note Returns after all operations in the queue have returned.
     */
    void run()
    {
        logger.info("(Server) start server");
        boost::lock_guard<boost::mutex> lock(mMutex);
        mThread = boost::thread([this]() {
            accept();
            mTcpIf.run();
            logger.info("(Server) stopped");
        });
    }

    /**
     * @fn stop
     * @brief Stop all connections.
     * @threadsafe
     */
    void stop()
    {
        logger.info("(Server) stopping all connections ...");
        {
            boost::lock_guard<boost::mutex> lock(mMutex);
            for(auto& it : mConnections)
            {
                if(it)
                {
                    it.reset();
                }
            }
            mTcpIf.stop();
        }
        if(mThread.joinable())
        {
            mThread.join();
        }
    }

    /**
     * @fn send
     * @brief Write a message to all clients.
     * @param crStr The msg to write
     * @threadsafe
     */
    void send(const std::string& crStr)
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        if(crStr.empty() || mActiveConnections == 0)
        {
            return;
        }
        for(auto& it : mConnections)
        {
            if(it)
            {
                if(!it.get()->write(crStr))
                {
                    logger.warn("(Server) lost connection to: ", it.get()->getIpAddress());
                    it.reset();
                    --mActiveConnections;
                }
            }
        }
    }

private:
    /**
     * @fn accept
     * @brief Accept connections.
     */
    void accept()
    {
        mTcpIf.onAccept(std::bind(&Server::attemptConnection, this, std::placeholders::_1));
    }

    /**
     * @fn isConnected
     * @brief Check whether an ip address already exists in the Connection container.
     * @param crIpAddress The ip address to check
     * @return true if the ip is already registered, else false
     */
    bool isConnected(const std::string& crIpAddress)
    {
        for(const auto& it : mConnections)
        {
            if(it && it.get()->getIpAddress() == crIpAddress)
            {
                return true;
            }
        }
        return false;
    }

    void attemptConnection(bool vError) noexcept
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        if(!vError)
        {
            if(mActiveConnections < S_MAX_CLIENTS && !isConnected(mTcpIf.getSocket().address()))
            {
                for(auto& it : mConnections)
                {
                    if(!it)
                    {
                        it = Connection<SocketT>::start(std::move(mTcpIf.getSocket()));
                        ++mActiveConnections;
                        logger.info("(Server) connection from: ", it->getIpAddress());
                        break;
                    }
                }
            }
            else
            {
                logger.info("(Server) refused connection to ", mTcpIf.getSocket().address());
                mTcpIf.close();
            }
        }
        else
        {
            logger.warn("(Server) Could not accept connection");
        }
        accept();
    }

    boost::thread mThread;

    /// @var mMutex
    /// Mutex
    boost::mutex mMutex;

    TcpInterfaceT mTcpIf;

    /// @var mConnections
    /// Vector holding Connections
    std::array<std::unique_ptr<Connection<SocketT>>, S_MAX_CLIENTS> mConnections;

    std::uint32_t mActiveConnections = 0;
};

}  // namespace server

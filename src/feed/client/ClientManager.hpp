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
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <unordered_set>
#include <boost/thread.hpp>
#include <boost/thread/lock_guard.hpp>
#include <boost/thread/mutex.hpp>

#include "../AprscFeed.h"
#include "../Feed.h"
#include "AprscClient.hpp"
#include "GpsdClient.hpp"
#include "SbsClient.hpp"
#include "SensorClient.hpp"

namespace feed
{
class Feed;
namespace client
{
template<typename ConnectorT>
struct ClientHasher
{
    std::size_t operator()(const std::shared_ptr<Client<ConnectorT>>& crClient) const
    {
        return crClient->hash();
    }
};

template<typename ConnectorT>
struct ClientComparator
{
    bool operator()(const std::shared_ptr<Client<ConnectorT>>& crClient1,
                    const std::shared_ptr<Client<ConnectorT>>& crClient2) const
    {
        return crClient1->equals(*crClient2);
    }
};

template<typename ConnectorT>
using ClientSet = std::unordered_set<std::shared_ptr<Client<ConnectorT>>, ClientHasher<ConnectorT>,
                                     ClientComparator<ConnectorT>>;

template<typename ConnectorT>
using ClientIter = typename ClientSet<ConnectorT>::iterator;

template<typename ConnectorT>
class ClientManager
{
public:
    enum class Protocol : std::uint32_t
    {
        APRS,
        SBS,
        GPS,
        SENSOR
    };

    ClientManager();

    ~ClientManager() noexcept;

    void subscribe(std::shared_ptr<Feed> rpFeed, const Endpoint& crEndpoint, Protocol vProtocol);

    void run();

    void stop();

private:
    ClientSet<ConnectorT> mClients;

    boost::thread_group mThdGroup;

    boost::mutex mMutex;
};

template<typename ConnectorT>
ClientManager<ConnectorT>::ClientManager()
{}

template<typename ConnectorT>
ClientManager<ConnectorT>::~ClientManager() noexcept
{}

template<typename ConnectorT>
void ClientManager<ConnectorT>::subscribe(std::shared_ptr<Feed> rpFeed, const Endpoint& crEndpoint,
                                          Protocol vProtocol)
{
    boost::lock_guard<boost::mutex> lock(mMutex);
    ClientIter<ConnectorT> it = mClients.end();
    switch(vProtocol)
    {
        case Protocol::APRS:
        {
            if(std::shared_ptr<AprscFeed> feed = std::dynamic_pointer_cast<AprscFeed>(rpFeed))
            {
                it = mClients
                         .insert(std::make_shared<AprscClient<ConnectorT>>(crEndpoint,
                                                                           feed->getLoginStr()))
                         .first;
            }
            else
            {
                throw std::invalid_argument("wrong protocol");
            }
        }
        break;
        case Protocol::SBS:
            it = mClients.insert(std::make_shared<SbsClient<ConnectorT>>(crEndpoint)).first;
            break;
        case Protocol::GPS:
            it = mClients.insert(std::make_shared<GpsdClient<ConnectorT>>(crEndpoint)).first;
            break;
        case Protocol::SENSOR:
            it = mClients.insert(std::make_shared<SensorClient<ConnectorT>>(crEndpoint)).first;
            break;
    }
    (*it)->subscribe(rpFeed);
}

template<typename ConnectorT>
void ClientManager<ConnectorT>::run()
{
    for(auto& it : mClients)
    {
        mThdGroup.create_thread([&]() {
            it->run();
            boost::lock_guard<boost::mutex> lock(mMutex);
            mClients.erase(it);
        });
    }
}

template<typename ConnectorT>
void ClientManager<ConnectorT>::stop()
{
    {
        boost::lock_guard<boost::mutex> lock(mMutex);
        for(const auto& it : mClients)
        {
            it->lockAndStop();
        }
    }
    mThdGroup.join_all();
}

}  // namespace client
}  // namespace feed

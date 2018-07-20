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
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <unordered_set>

#include "../feed/AprscFeed.h"
#include "../feed/Feed.h"
#include "AprscClient.hpp"
#include "GpsdClient.hpp"
#include "SbsClient.hpp"
#include "SensorClient.hpp"

struct thread_group
{
    void create_thread(const std::function<void()>& crFunc)
    {
        _threads.push_back(std::thread(crFunc));
    }

    void join_all()
    {
        std::for_each(_threads.begin(), _threads.end(), [](std::thread& rThd) {
            if(rThd.joinable())
            {
                rThd.join();
            }
        });
    }

private:
    std::list<std::thread> _threads;
};

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
    ClientManager();

    ~ClientManager() noexcept;

    void subscribe(std::shared_ptr<feed::Feed> pFeed);

    void run();

    void stop();

private:
    ClientSet<ConnectorT> mClients;

    thread_group mThdGroup;

    std::mutex mMutex;
};

template<typename ConnectorT>
ClientManager<ConnectorT>::ClientManager()
{}

template<typename ConnectorT>
ClientManager<ConnectorT>::~ClientManager() noexcept
{
    stop();
}

template<typename ConnectorT>
void ClientManager<ConnectorT>::subscribe(std::shared_ptr<feed::Feed> pFeed)
{
    std::lock_guard<std::mutex> lock(mMutex);
    ClientIter<ConnectorT> it = mClients.end();
    Endpoint endpoint         = pFeed->getEndpoint();
    switch(pFeed->getProtocol())
    {
        case feed::Feed::Protocol::APRS:
            it = mClients
                     .insert(std::make_shared<AprscClient<ConnectorT>>(
                         endpoint, std::static_pointer_cast<feed::AprscFeed>(pFeed)->getLoginStr()))
                     .first;
            break;
        case feed::Feed::Protocol::SBS:
            it = mClients.insert(std::make_shared<SbsClient<ConnectorT>>(endpoint)).first;
            break;
        case feed::Feed::Protocol::GPS:
            it = mClients.insert(std::make_shared<GpsdClient<ConnectorT>>(endpoint)).first;
            break;
        case feed::Feed::Protocol::SENSOR:
            it = mClients.insert(std::make_shared<SensorClient<ConnectorT>>(endpoint)).first;
            break;
    }
    (*it)->subscribe(pFeed);
}

template<typename ConnectorT>
void ClientManager<ConnectorT>::run()
{
    for(auto& it : mClients)
    {
        mThdGroup.create_thread([&]() {
            it->run();
            std::lock_guard<std::mutex> lock(mMutex);
            mClients.erase(it);
        });
    }
}

template<typename ConnectorT>
void ClientManager<ConnectorT>::stop()
{
    {
        std::lock_guard<std::mutex> lock(mMutex);
        for(auto& it : mClients)
        {
            it->lockAndStop();
        }
    }
    mThdGroup.join_all();
}

}  // namespace client

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
#include <unordered_set>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "Client.hpp"

namespace feed
{
class Feed;
namespace client
{
struct ClientHasher
{
    std::size_t operator()(const std::shared_ptr<Client>& crClient) const
    {
        return crClient->hash();
    }
};

struct ClientComparator
{
    bool operator()(const std::shared_ptr<Client>& crClient1,
                    const std::shared_ptr<Client>& crClient2) const
    {
        return crClient1->equals(*crClient2);
    }
};

using ClientSet = std::unordered_set<std::shared_ptr<Client>, ClientHasher, ClientComparator>;

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

    void subscribe(std::shared_ptr<Feed> rpFeed, const Endpoint& crEndpoint,
                                    Protocol vProtocol);

    void run();

    void stop();

private:
    ClientSet mClients;

    boost::thread_group mThdGroup;

    boost::mutex mMutex;
};
}  // namespace client
}  // namespace feed

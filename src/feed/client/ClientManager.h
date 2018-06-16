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

#include <iterator>
#include <memory>
#include <unordered_set>

#include "../../Defines.h"
#include "Client.h"

namespace boost
{
class thread_group;
}  // namespace boost

namespace feed
{
class Feed;
namespace client
{
using ClientSet
    = std::unordered_set<std::unique_ptr<Client>, ClientHasher, ClientComparator>;

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

    DEFAULT_CTOR_DTOR(ClientManager)

    ClientSet::iterator subscribe(std::shared_ptr<Feed> rpFeed,
                                  const Endpoint& crEndpoint, Protocol vProtocol);

    void run(boost::thread_group& rThdGroup, boost::asio::signal_set& rSigset);

private:
    ClientSet mClients;
};
}  // namespace client
}  // namespace feed

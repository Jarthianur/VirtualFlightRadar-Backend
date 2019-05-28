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

#include <memory>

#include "Client.h"

namespace feed
{
class Feed;
}  // namespace feed

namespace client
{
/**
 * @brief A factory for clients.
 */
class ClientFactory
{
    //< begin methods >//
    /**
     * @brief Factory method for Client creation.
     * @tparam T The type of client
     * @param feed The feed to create for
     * @return the client as pointer
     */
    template<typename T, typename std::enable_if<std::is_base_of<Client, T>::value>::type* = nullptr>
    static std::shared_ptr<T> makeClient(std::shared_ptr<feed::Feed> feed);
    //< end methods >//

public:
    ClientFactory()           = default;
    ~ClientFactory() noexcept = default;

    //< begin interfaces >//
    /**
     * @brief Create a Client needed by a Feed.
     * @param feed The feed to create for
     * @return the client as pointer
     */
    static std::shared_ptr<Client> createClientFor(std::shared_ptr<feed::Feed> feed);
    //< end interfaces >//
};

}  // namespace client

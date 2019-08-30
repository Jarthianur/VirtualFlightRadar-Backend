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

#include "error/Error.hpp"
#include "util/class_utils.h"

#include "Client.h"
#include "types.h"

namespace vfrb::feed
{
class Feed;
}  // namespace vfrb::feed

namespace vfrb::client
{
/**
 * @brief A factory for clients.
 */
class ClientFactory
{
    /**
     * @brief Factory method for Client creation.
     * @tparam T The type of client
     * @param feed The feed to create for
     * @return the client as pointer
     */
    template<typename T, ENABLE_IF(EXTENDS(T, Client))>
    static s_ptr<T> makeClient(s_ptr<feed::Feed> feed);

public:
    ClientFactory()           = default;
    ~ClientFactory() noexcept = default;

    /**
     * @brief Create a Client needed by a Feed.
     * @param feed The feed to create for
     * @return the client as pointer
     */
    static s_ptr<Client> createClientFor(s_ptr<feed::Feed> feed);
};

namespace error
{
class NoSuchProtocolError : public vfrb::error::Error
{
public:
    NoSuchProtocolError()                    = default;
    ~NoSuchProtocolError() noexcept override = default;

    char const* what() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::client

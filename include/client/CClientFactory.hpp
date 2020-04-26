/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "error/IError.hpp"
#include "util/class_utils.hpp"

#include "IClient.hpp"
#include "types.hpp"

namespace vfrb::feed
{
class IFeed;
}  // namespace vfrb::feed

namespace vfrb::client
{
/**
 * @brief A factory for clients.
 */
class CClientFactory
{
    /**
     * @brief Factory method for Client creation.
     * @tparam T The type of client
     * @param feed The feed to create for
     * @return the client as pointer
     */
    template<typename T, ENABLE_IF(EXTENDS(T, IClient))>
    static SPtr<T> makeClient(SPtr<feed::IFeed> feed_);

public:
    /**
     * @brief Create a Client needed by a Feed.
     * @param feed The feed to create for
     * @return the client as pointer
     */
    static SPtr<IClient> CreateClientFor(SPtr<feed::IFeed> feed_);
};

namespace error
{
class CNoSuchProtocolError : public vfrb::error::IError
{
public:
    str Message() const noexcept override;
};
}  // namespace error
}  // namespace vfrb::client

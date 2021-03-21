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

#include "client/CClientFactory.hpp"

#include "client/CAprscClient.hpp"
#include "client/CGpsdClient.hpp"
#include "client/CSbsClient.hpp"
#include "client/CSensorClient.hpp"
#include "client/net/impl/CConnectorAsio.hpp"
#include "feed/CAprscFeed.hpp"
#include "feed/IFeed.hpp"

using vfrb::feed::IFeed;
using vfrb::feed::CAprscFeed;
using vfrb::client::net::CConnectorAsio;

namespace vfrb::client
{
template<>
auto
CClientFactory::makeClient<CAprscClient>(Shared<IFeed> feed_) -> Shared<CAprscClient> {
    return AllocShared<CAprscClient>(feed_->Endpoint(), std::static_pointer_cast<CAprscFeed>(feed_)->Login(),
                                     AllocShared<CConnectorAsio>());
}

template<>
auto
CClientFactory::makeClient<CSbsClient>(Shared<IFeed> feed_) -> Shared<CSbsClient> {
    return AllocShared<CSbsClient>(feed_->Endpoint(), AllocShared<CConnectorAsio>());
}

template<>
auto
CClientFactory::makeClient<CSensorClient>(Shared<IFeed> feed_) -> Shared<CSensorClient> {
    return AllocShared<CSensorClient>(feed_->Endpoint(), AllocShared<CConnectorAsio>());
}

template<>
auto
CClientFactory::makeClient<CGpsdClient>(Shared<IFeed> feed_) -> Shared<CGpsdClient> {
    return AllocShared<CGpsdClient>(feed_->Endpoint(), AllocShared<CConnectorAsio>());
}

auto
CClientFactory::CreateClientFor(Shared<IFeed> feed_) -> Shared<IClient> {
    switch (feed_->Protocol()) {
        case IFeed::EProtocol::APRS: return makeClient<CAprscClient>(feed_);
        case IFeed::EProtocol::SBS: return makeClient<CSbsClient>(feed_);
        case IFeed::EProtocol::GPS: return makeClient<CGpsdClient>(feed_);
        case IFeed::EProtocol::SENSOR: return makeClient<CSensorClient>(feed_);
    }
    throw error::CNoSuchProtocolError();
}

namespace error
{
auto
CNoSuchProtocolError::Message() const noexcept -> str {
    return "unknown protocol";
}
}  // namespace error
}  // namespace vfrb::client

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

#include "client/ClientFactory.h"

#include "client/AprscClient.h"
#include "client/GpsdClient.h"
#include "client/SbsClient.h"
#include "client/SensorClient.h"
#include "client/net/impl/ConnectorBoost.h"
#include "feed/AprscFeed.h"
#include "feed/Feed.h"

using namespace vfrb::client::net;
using namespace vfrb::feed;

namespace vfrb::client
{
template<>
SPtr<CAprscClient> CClientFactory::makeClient<CAprscClient>(SPtr<IFeed> feed_)
{
    return std::make_shared<CAprscClient>(feed_->Endpoint(),
                                          std::static_pointer_cast<CAprscFeed>(feed_)->Login(),
                                          std::make_shared<CConnectorBoost>());
}

template<>
SPtr<CSbsClient> CClientFactory::makeClient<CSbsClient>(SPtr<IFeed> feed_)
{
    return std::make_shared<CSbsClient>(feed_->Endpoint(), std::make_shared<CConnectorBoost>());
}

template<>
SPtr<CSensorClient> CClientFactory::makeClient<CSensorClient>(SPtr<IFeed> feed_)
{
    return std::make_shared<CSensorClient>(feed_->Endpoint(), std::make_shared<CConnectorBoost>());
}

template<>
SPtr<CGpsdClient> CClientFactory::makeClient<CGpsdClient>(SPtr<IFeed> feed_)
{
    return std::make_shared<CGpsdClient>(feed_->Endpoint(), std::make_shared<CConnectorBoost>());
}

SPtr<IClient> CClientFactory::CreateClientFor(SPtr<IFeed> feed_)
{
    switch (feed_->Protocol())
    {
        case IFeed::EProtocol::APRS: return makeClient<CAprscClient>(feed_);
        case IFeed::EProtocol::SBS: return makeClient<CSbsClient>(feed_);
        case IFeed::EProtocol::GPS: return makeClient<CGpsdClient>(feed_);
        case IFeed::EProtocol::SENSOR: return makeClient<CSensorClient>(feed_);
    }
    throw error::CNoSuchProtocolError();
}

namespace error
{
char const* CNoSuchProtocolError::Message() const noexcept
{
    return "unknown protocol";
}
}  // namespace error
}  // namespace vfrb::client

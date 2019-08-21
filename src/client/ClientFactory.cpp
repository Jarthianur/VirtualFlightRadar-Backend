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
#include "client/net/impl/ConnectorImplBoost.h"
#include "feed/AprscFeed.h"
#include "feed/Feed.h"

namespace client
{
using namespace net;

template<>
std::shared_ptr<AprscClient> ClientFactory::makeClient<AprscClient>(std::shared_ptr<feed::Feed> feed)
{
    return std::make_shared<AprscClient>(feed->endpoint(),
                                         std::static_pointer_cast<feed::AprscFeed>(feed)->login(),
                                         std::make_shared<ConnectorImplBoost>());
}

template<>
std::shared_ptr<SbsClient> ClientFactory::makeClient<SbsClient>(std::shared_ptr<feed::Feed> feed)
{
    return std::make_shared<SbsClient>(feed->endpoint(), std::make_shared<ConnectorImplBoost>());
}

template<>
std::shared_ptr<SensorClient> ClientFactory::makeClient<SensorClient>(std::shared_ptr<feed::Feed> feed)
{
    return std::make_shared<SensorClient>(feed->endpoint(), std::make_shared<ConnectorImplBoost>());
}

template<>
std::shared_ptr<GpsdClient> ClientFactory::makeClient<GpsdClient>(std::shared_ptr<feed::Feed> feed)
{
    return std::make_shared<GpsdClient>(feed->endpoint(), std::make_shared<ConnectorImplBoost>());
}

std::shared_ptr<Client> ClientFactory::createClientFor(std::shared_ptr<feed::Feed> feed)
{
    switch (feed->protocol())
    {
        case feed::Feed::Protocol::APRS: return makeClient<AprscClient>(feed);
        case feed::Feed::Protocol::SBS: return makeClient<SbsClient>(feed);
        case feed::Feed::Protocol::GPS: return makeClient<GpsdClient>(feed);
        case feed::Feed::Protocol::SENSOR: return makeClient<SensorClient>(feed);
    }
    throw std::logic_error("unknown protocol");  // can never happen
}
}  // namespace client

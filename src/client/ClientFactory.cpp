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

using namespace vfrb::client::net;
using namespace vfrb::feed;

namespace vfrb::client
{
template<>
s_ptr<AprscClient> ClientFactory::makeClient<AprscClient>(s_ptr<Feed> feed)
{
    return std::make_shared<AprscClient>(feed->endpoint(), std::static_pointer_cast<AprscFeed>(feed)->login(),
                                         std::make_shared<ConnectorImplBoost>());
}

template<>
s_ptr<SbsClient> ClientFactory::makeClient<SbsClient>(s_ptr<Feed> feed)
{
    return std::make_shared<SbsClient>(feed->endpoint(), std::make_shared<ConnectorImplBoost>());
}

template<>
s_ptr<SensorClient> ClientFactory::makeClient<SensorClient>(s_ptr<Feed> feed)
{
    return std::make_shared<SensorClient>(feed->endpoint(), std::make_shared<ConnectorImplBoost>());
}

template<>
s_ptr<GpsdClient> ClientFactory::makeClient<GpsdClient>(s_ptr<Feed> feed)
{
    return std::make_shared<GpsdClient>(feed->endpoint(), std::make_shared<ConnectorImplBoost>());
}

s_ptr<Client> ClientFactory::createClientFor(s_ptr<Feed> feed)
{
    switch (feed->protocol())
    {
        case Feed::Protocol::APRS: return makeClient<AprscClient>(feed);
        case Feed::Protocol::SBS: return makeClient<SbsClient>(feed);
        case Feed::Protocol::GPS: return makeClient<GpsdClient>(feed);
        case Feed::Protocol::SENSOR: return makeClient<SensorClient>(feed);
    }
    throw error::NoSuchProtocolError();
}

namespace error
{
char const* NoSuchProtocolError::what() const noexcept
{
    return "unknown protocol";
}
}  // namespace error
}  // namespace vfrb::client

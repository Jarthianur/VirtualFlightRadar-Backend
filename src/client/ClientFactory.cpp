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

#include "ClientFactory.h"

#include "../feed/AprscFeed.h"
#include "../feed/Feed.h"

#include "AprscClient.h"
#include "ConnectorImplBoost.h"
#include "GpsdClient.h"
#include "SbsClient.h"
#include "SensorClient.h"

namespace client
{
ClientFactory::ClientFactory() {}

ClientFactory::~ClientFactory() noexcept {}

template<>
std::shared_ptr<AprscClient>
    ClientFactory::makeClient<AprscClient>(std::shared_ptr<feed::Feed> feed)
{
    return std::make_shared<AprscClient>(
        feed->get_endpoint(), std::static_pointer_cast<feed::AprscFeed>(feed)->get_login(),
        std::make_shared<ConnectorImplBoost>());
}

template<>
std::shared_ptr<SbsClient> ClientFactory::makeClient<SbsClient>(std::shared_ptr<feed::Feed> feed)
{
    return std::make_shared<SbsClient>(feed->get_endpoint(),
                                       std::make_shared<ConnectorImplBoost>());
}

template<>
std::shared_ptr<SensorClient>
    ClientFactory::makeClient<SensorClient>(std::shared_ptr<feed::Feed> feed)
{
    return std::make_shared<SensorClient>(feed->get_endpoint(),
                                          std::make_shared<ConnectorImplBoost>());
}

template<>
std::shared_ptr<GpsdClient> ClientFactory::makeClient<GpsdClient>(std::shared_ptr<feed::Feed> feed)
{
    return std::make_shared<GpsdClient>(feed->get_endpoint(),
                                        std::make_shared<ConnectorImplBoost>());
}

std::shared_ptr<Client> ClientFactory::createClientFor(std::shared_ptr<feed::Feed> feed)
{
    switch (feed->get_protocol())
    {
        case feed::Feed::Protocol::APRS: return makeClient<AprscClient>(feed);
        case feed::Feed::Protocol::SBS: return makeClient<SbsClient>(feed);
        case feed::Feed::Protocol::GPS: return makeClient<GpsdClient>(feed);
        case feed::Feed::Protocol::SENSOR: return makeClient<SensorClient>(feed);
    }
    throw std::logic_error("unknown protocol");  // can never happen
}

}  // namespace client

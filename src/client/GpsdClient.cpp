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

#include "client/GpsdClient.h"

#include <chrono>
#include <condition_variable>
#include <thread>
#include <utility>

#include "util/Logger.hpp"

namespace client
{
using namespace net;

constexpr auto LOG_PREFIX = "(GpsdClient) ";

GpsdClient::GpsdClient(const Endpoint& endpoint, std::shared_ptr<Connector> connector)
    : Client(endpoint, LOG_PREFIX, connector)
{}

void GpsdClient::handleConnect(bool error)
{
    if (!error)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connector->onWrite("?WATCH={\"enable\":true,\"nmea\":true}\r\n",
                             std::bind(&GpsdClient::handleWatch, this, std::placeholders::_1));
    }
    else
    {
        logger.warn(LOG_PREFIX, "failed to connect to ", m_endpoint.host, ":", m_endpoint.port);
        reconnect();
    }
}

void GpsdClient::stop()
{
    std::mutex                   sync;
    std::unique_lock<std::mutex> lock(sync);
    std::condition_variable      cv;
    bool                         sent = false;
    if (m_running)
    {
        m_connector->onWrite("?WATCH={\"enable\":false}\r\n", [&sent, &cv](bool) {
            logger.info(LOG_PREFIX, "stopped watch");
            sent = true;
            cv.notify_one();
        });
    }
    cv.wait_for(lock, std::chrono::milliseconds(500), [&] { return sent; });
    Client::stop();
}

void GpsdClient::handleWatch(bool error)
{
    if (!error)
    {
        logger.info(LOG_PREFIX, "connected to ", m_endpoint.host, ":", m_endpoint.port);
        std::lock_guard<std::mutex> lock(m_mutex);
        read();
    }
    else
    {
        logger.error(LOG_PREFIX, "send watch request failed");
    }
}
}  // namespace client

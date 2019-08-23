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

#include "feed/Feed.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <unordered_map>

#include "config/Configuration.h"
#include "data/Data.hpp"
#include "util/Logger.hpp"

using namespace config;

namespace feed
{
static auto const& logger = Logger::instance();

Feed::Feed(str const& name, char const* logPrefix, Properties const& properties,
           std::shared_ptr<data::Data> data)
    : m_data(data), m_priority(initPriority()), m_logPrefix(logPrefix), name(name), properties(properties)
{
    if (properties.property(Configuration::KV_KEY_HOST).empty())
    {
        logger.warn(m_logPrefix, "could not find: ", name, ".", Configuration::KV_KEY_HOST);
        throw std::logic_error("No host given");
    }
    if (properties.property(Configuration::KV_KEY_PORT).empty())
    {
        logger.warn(m_logPrefix, "could not find: ", name, ".", Configuration::KV_KEY_PORT);
        throw std::logic_error("No port given");
    }
}

u32 Feed::initPriority() const noexcept
{
    try
    {
        return static_cast<u32>(
            std::max<u64>(0, std::min<u64>(std::stoul(properties.property(Configuration::KV_KEY_PRIORITY)),
                                           std::numeric_limits<u32>::max())));
    }
    catch (std::logic_error const&)
    {
        logger.warn(m_logPrefix, "create ", name, ": Invalid priority given.");
    }
    return 0;
}

client::net::Endpoint Feed::endpoint() const
{
    return {properties.property(Configuration::KV_KEY_HOST), properties.property(Configuration::KV_KEY_PORT)};
}
}  // namespace feed

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
Feed::Feed(const std::string& name, const char* component, const Properties& properties,
           std::shared_ptr<data::Data> data)
    : m_name(name), m_component(component), m_properties(properties), m_data(data)
{
    initPriority();
    if (m_properties.get_property(KV_KEY_HOST).empty())
    {
        logger.warn(m_component, " could not find: ", m_name, "." KV_KEY_HOST);
        throw std::logic_error("No host given");
    }
    if (m_properties.get_property(KV_KEY_PORT).empty())
    {
        logger.warn(m_component, " could not find: ", m_name, "." KV_KEY_PORT);
        throw std::logic_error("No port given");
    }
}

void Feed::initPriority() noexcept
{
    try
    {
        m_priority = static_cast<std::uint32_t>(std::max<std::uint64_t>(
            0, std::min<std::uint64_t>(std::stoul(m_properties.get_property(KV_KEY_PRIORITY)),
                                       std::numeric_limits<std::uint32_t>::max())));
    }
    catch (const std::logic_error&)
    {
        logger.warn(m_component, " create ", m_name, ": Invalid priority given.");
    }
}

client::Endpoint Feed::get_endpoint() const
{
    return {m_properties.get_property(KV_KEY_HOST), m_properties.get_property(KV_KEY_PORT)};
}

}  // namespace feed

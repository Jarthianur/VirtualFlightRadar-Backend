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

#include "Feed.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <unordered_map>

#include "../Logger.hpp"
#include "../config/Configuration.h"
#include "../data/Data.h"

#ifdef COMPONENT
#undef COMPONENT
#endif
#define COMPONENT "(Feed)"

namespace feed
{
Feed::Feed(const std::string& name, const config::KeyValueMap& propertyMap,
           std::shared_ptr<data::Data> data)
    : m_name(name), m_propertyMap(propertyMap), m_data(data)
{
    initPriority();
    if(m_propertyMap.find(KV_KEY_HOST) == m_propertyMap.end())
    {
        logger.warn(COMPONENT " could not find: ", m_name, "." KV_KEY_HOST);
        throw std::logic_error("No host given");
    }
    if(m_propertyMap.find(KV_KEY_PORT) == m_propertyMap.end())
    {
        logger.warn(COMPONENT " could not find: ", m_name, "." KV_KEY_PORT);
        throw std::logic_error("No port given");
    }
}

Feed::~Feed() noexcept
{}

void Feed::initPriority() noexcept
{
    try
    {
        m_priority = static_cast<std::uint32_t>(std::max<std::uint64_t>(
            0, std::min<std::uint64_t>(std::stoul(m_propertyMap.at(KV_KEY_PRIORITY)),
                                       std::numeric_limits<std::uint32_t>::max())));
    }
    catch(const std::logic_error&)
    {
        logger.warn(COMPONENT " create ", m_name, ": Invalid priority given.");
    }
    if(m_priority == 0)
    {
        logger.warn(COMPONENT " create ", m_name,
                    ": Priority is 0; this feed cannot update higher ones.");
    }
}

client::Endpoint Feed::get_endpoint() const
{
    return {m_propertyMap.find(KV_KEY_HOST)->second, m_propertyMap.find(KV_KEY_PORT)->second};
}

}  // namespace feed

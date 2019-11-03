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

using namespace vfrb::config;
using namespace std::literals;

namespace vfrb::feed
{
Feed::Feed(str const& name, Properties const& properties, s_ptr<data::Data> data)
    : m_properties(properties), m_name(name), m_priority(initPriority()), m_data(data)
{
    if (properties.property(Configuration::KV_KEY_HOST, "").empty())
    {
        throw error::InvalidPropertyError("could not find: "s + name + "." + Configuration::KV_KEY_HOST);
    }
    if (properties.property(Configuration::KV_KEY_PORT, "").empty())
    {
        throw error::InvalidPropertyError("could not find: "s + name + "." + Configuration::KV_KEY_PORT);
    }
}

u32 Feed::initPriority() const
{
    try
    {
        return static_cast<u32>(std::max<u64>(
            0, std::min<u64>(std::stoul(m_properties.property(Configuration::KV_KEY_PRIORITY, "0")),
                             std::numeric_limits<u32>::max())));
    }
    catch ([[maybe_unused]] std::logic_error const&)
    {}
    throw error::InvalidPropertyError("invalid priority given");
}

client::net::Endpoint Feed::endpoint() const
{
    return {m_properties.property(Configuration::KV_KEY_HOST),
            m_properties.property(Configuration::KV_KEY_PORT)};
}

auto Feed::name() const -> decltype(m_name) const&
{
    return m_name;
}

auto Feed::priority() const -> std::remove_const<decltype(m_priority)>::type
{
    return m_priority;
}

namespace error
{
InvalidPropertyError::InvalidPropertyError(str const& msg) : m_msg(msg) {}

char const* InvalidPropertyError::what() const noexcept
{
    return m_msg.c_str();
}
}  // namespace error
}  // namespace vfrb::feed

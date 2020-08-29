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

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <unordered_map>

#include "config/CConfiguration.hpp"
#include "config/CProperties.hpp"
#include "data/IData.hpp"
#include "feed/IFeed.hpp"
#include "util/StringUtils.hpp"

using vfrb::config::CProperties;
using vfrb::config::CConfiguration;

using vfrb::str_util::MakeStr;

namespace vfrb::feed
{
IFeed::IFeed(String const& name_, CProperties const& prop_, SPtr<data::IData> data_)
    : m_properties(prop_), m_name(name_), m_priority(initPriority()), m_data(data_) {
    if (prop_.Property(CConfiguration::KV_KEY_HOST, "").empty()) {
        throw error::CInvalidPropertyError(
          MakeStr("could not find: ", name_, ".", CConfiguration::KV_KEY_HOST));
    }
    if (prop_.Property(CConfiguration::KV_KEY_PORT, "").empty()) {
        throw error::CInvalidPropertyError(
          MakeStr("could not find: ", name_, ".", CConfiguration::KV_KEY_PORT));
    }
}

auto
IFeed::initPriority() const -> u32 {
    try {
        return static_cast<u32>(std::max<u64>(
          0, std::min<u64>(std::stoul(m_properties.Property(CConfiguration::KV_KEY_PRIORITY, "0")),
                           std::numeric_limits<u32>::max())));
    } catch ([[maybe_unused]] std::logic_error const&) {
    }
    throw error::CInvalidPropertyError("invalid priority given");
}

auto
IFeed::Endpoint() const -> client::net::SEndpoint {
    return {m_properties.Property(CConfiguration::KV_KEY_HOST),
            m_properties.Property(CConfiguration::KV_KEY_PORT)};
}

auto
IFeed::Priority() const -> std::remove_const<decltype(m_priority)>::type {
    return m_priority;
}

namespace error
{
CInvalidPropertyError::CInvalidPropertyError(String const& msg_) : m_msg(msg_) {}

auto
CInvalidPropertyError::Message() const noexcept -> str {
    return m_msg.c_str();
}
}  // namespace error
}  // namespace vfrb::feed

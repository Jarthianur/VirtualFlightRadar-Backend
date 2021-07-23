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

#include "feed/CAprscFeed.hpp"

#include "config/CConfiguration.hpp"
#include "config/CProperties.hpp"
#include "data/CAircraftData.hpp"
#include "object/CAircraft.hpp"
#include "util/StringUtils.hpp"

using vfrb::config::CProperties;
using vfrb::config::CConfiguration;
using vfrb::str_util::MakeString;

namespace vfrb::feed
{
CAprscFeed::CAprscFeed(String const& name_, CProperties const& prop_, Shared<data::CAircraftData> data_,
                       i32 maxHeight_)
    : IFeed(name_, prop_, data_), m_parser(maxHeight_), m_worker([this](String&& work) {
          try {
              m_data->Update(m_parser.Parse(std::move(work), m_priority));
          } catch ([[maybe_unused]] parser::error::CParseError const&) {
          }
      }) {
    try {
        [[maybe_unused]] auto x = prop_.Property(CConfiguration::KV_KEY_LOGIN);
    } catch ([[maybe_unused]] config::error::CPropertyNotFoundError const&) {
        throw error::CInvalidPropertyError(
            MakeString("could not find: ", name_, ".", CConfiguration::KV_KEY_LOGIN));
    }
}

auto
CAprscFeed::Protocol() const -> IFeed::EProtocol {
    return EProtocol::APRS;
}

auto
CAprscFeed::Consume(String&& str_) -> bool {
    m_worker.Put(std::move(str_));
    return true;
}

auto
CAprscFeed::Login() const -> String {
    return m_properties.Property(CConfiguration::KV_KEY_LOGIN);
}
}  // namespace vfrb::feed

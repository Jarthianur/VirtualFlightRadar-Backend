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

#include "feed/CWindFeed.hpp"

#include "config/CConfiguration.hpp"
#include "config/CProperties.hpp"
#include "data/CWindData.hpp"
#include "feed/parser/CWindParser.hpp"
#include "object/CWind.hpp"

using vfrb::config::CProperties;

namespace vfrb::feed
{
CWindFeed::CWindFeed(String const& name_, CProperties const& prop_, Shared<data::CWindData> data_)
    : IFeed(name_, prop_, data_) {}

auto
CWindFeed::Protocol() const -> IFeed::EProtocol {
    return EProtocol::SENSOR;
}

auto
CWindFeed::Consume(String&& str_) -> bool {
    try {
        m_data->Update(m_parser.Parse(std::move(str_), m_priority));
    } catch ([[maybe_unused]] parser::error::CParseError const&) {
    }
    return true;
}
}  // namespace vfrb::feed

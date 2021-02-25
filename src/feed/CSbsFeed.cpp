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

#include "feed/CSbsFeed.hpp"

#include "config/CConfiguration.hpp"
#include "config/CProperties.hpp"
#include "data/CAircraftData.hpp"
#include "feed/parser/CSbsParser.hpp"
#include "object/CAircraft.hpp"

using vfrb::config::CProperties;

namespace vfrb::feed
{
CSbsFeed::CSbsFeed(String const& name_, CProperties const& prop_, SPtr<data::CAircraftData> data_,
                   i32 maxHeight_)
    : IFeed(name_, prop_, data_), m_parser(maxHeight_), m_worker([this](String&& work_) {
          try {
              m_data->Update(m_parser.Parse(std::move(work_), m_priority));
          } catch ([[maybe_unused]] parser::error::CParseError const&) {
          }
      }) {}

auto
CSbsFeed::Protocol() const -> IFeed::EProtocol {
    return EProtocol::SBS;
}

auto
CSbsFeed::Process(String&& str_) -> bool {
    m_worker.Push(std::move(str_));
    return true;
}
}  // namespace vfrb::feed

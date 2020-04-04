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

#include "feed/CSbsFeed.hpp"

#include "config/CConfiguration.hpp"
#include "data/CAircraftData.hpp"
#include "feed/parser/CSbsParser.hpp"
#include "object/CAircraft.hpp"

using namespace vfrb::config;

namespace vfrb::feed
{
CSbsFeed::CSbsFeed(String const& name_, CProperties const& prop_, SPtr<data::CAircraftData> data_,
                   s32 maxHeight_)
    : IFeed(name_, prop_, data_), m_parser(maxHeight_), m_worker([this](String&& work_) {
          try {
              m_data->Update(m_parser.Parse(std::move(work_), m_priority));
          } catch ([[maybe_unused]] parser::error::CParseError const&) {
          }
      }) {}

IFeed::EProtocol CSbsFeed::Protocol() const {
    return EProtocol::SBS;
}

bool CSbsFeed::Process(String str_) {
    m_worker.Push(std::move(str_));
    return true;
}
}  // namespace vfrb::feed

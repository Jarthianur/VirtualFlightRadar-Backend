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

#include "feed/AprscFeed.h"

#include <stdexcept>

#include "config/Configuration.h"
#include "data/AircraftData.h"
#include "object/Aircraft.h"

using namespace vfrb::config;
using namespace std::literals;

namespace vfrb::feed
{
CAprscFeed::CAprscFeed(Str const& name_, CProperties const& prop_, SPtr<data::CAircraftData> data_,
                       s32 maxHeight_)
    : IFeed(name_, prop_, data_), m_parser(maxHeight_), m_worker([this](Str&& work) {
          try
          {
              m_data->Update(m_parser.Parse(std::move(work), m_priority));
          }
          catch ([[maybe_unused]] parser::error::CParseError const&)
          {}
      })
{
    try
    {
        prop_.Property(CConfiguration::KV_KEY_LOGIN);
    }
    catch ([[maybe_unused]] config::error::CPropertyNotFoundError const&)
    {
        throw error::CInvalidPropertyError("could not find: "s + name_ + "." + CConfiguration::KV_KEY_LOGIN);
    }
}

IFeed::EProtocol CAprscFeed::Protocol() const
{
    return EProtocol::APRS;
}

bool CAprscFeed::Process(Str str_)
{
    m_worker.Push(std::move(str_));
    return true;
}

Str CAprscFeed::Login() const
{
    return m_properties.Property(CConfiguration::KV_KEY_LOGIN);
}
}  // namespace vfrb::feed

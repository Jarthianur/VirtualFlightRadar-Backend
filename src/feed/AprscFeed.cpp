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
AprscFeed::AprscFeed(Str const& name, CProperties const& properties, SPtr<data::CAircraftData> data,
                     s32 maxHeight)
    : Feed(name, properties, data), m_parser(maxHeight), m_worker([this](Str&& work) {
          try
          {
              m_data->update(m_parser.unpack(std::move(work), m_priority));
          }
          catch ([[maybe_unused]] parser::error::UnpackError const&)
          {}
      })
{
    try
    {
        properties.Property(CConfiguration::KV_KEY_LOGIN);
    }
    catch ([[maybe_unused]] config::error::CPropertyNotFoundError const&)
    {
        throw error::InvalidPropertyError("could not find: "s + name + "." + CConfiguration::KV_KEY_LOGIN);
    }
}

Feed::Protocol AprscFeed::protocol() const
{
    return Protocol::APRS;
}

bool AprscFeed::process(Str response)
{
    m_worker.push(std::move(response));
    return true;
}

Str AprscFeed::login() const
{
    return m_properties.Property(CConfiguration::KV_KEY_LOGIN);
}
}  // namespace vfrb::feed

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

#include "feed/SbsFeed.h"

#include "config/Configuration.h"
#include "data/AircraftData.h"
#include "feed/parser/SbsParser.h"
#include "object/Aircraft.h"

using namespace vfrb::config;

namespace vfrb::feed
{
parser::SbsParser SbsFeed::s_parser;

SbsFeed::SbsFeed(str const& name, Properties const& properties, s_ptr<data::AircraftData> data, s32 maxHeight)
    : Feed(name, properties, data), m_worker([this](str&& work) {
          try
          {
              m_data->update(s_parser.unpack(work, m_priority));
          }
          catch (parser::UnpackError const&)
          {}
      })
{
    parser::SbsParser::s_maxHeight = maxHeight;
}

Feed::Protocol SbsFeed::protocol() const
{
    return Protocol::SBS;
}

bool SbsFeed::process(str const& response)
{
    m_worker.push(response);
    return true;
}
}  // namespace vfrb::feed

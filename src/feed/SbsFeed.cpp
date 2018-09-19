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

#include "SbsFeed.h"

#include <unordered_map>

#include "../config/Configuration.h"
#include "../data/AircraftData.h"
#include "../object/Aircraft.h"
#include "parser/SbsParser.h"

namespace feed
{
parser::SbsParser SbsFeed::smParser;

SbsFeed::SbsFeed(const std::string& crName, const config::KeyValueMap& crKvMap,
                 std::shared_ptr<data::AircraftData> pData, std::int32_t vMaxHeight)
    : Feed(crName, crKvMap, pData)
{
    parser::SbsParser::s_maxHeight = vMaxHeight;
}

SbsFeed::~SbsFeed() noexcept
{}

Feed::Protocol SbsFeed::get_protocol() const
{
    return Protocol::SBS;
}

bool SbsFeed::process(const std::string& crResponse) noexcept
{
    object::Aircraft ac(get_priority());
    if(smParser.unpack(crResponse, ac))
    {
        m_data->update(std::move(ac));
    }
    return true;
}

}  // namespace feed

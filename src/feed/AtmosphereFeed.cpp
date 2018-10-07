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

#include "AtmosphereFeed.h"

#include <unordered_map>

#include "../config/Configuration.h"
#include "../data/AtmosphereData.h"
#include "../object/Atmosphere.h"
#include "../util/Logger.hpp"
#include "parser/AtmosphereParser.h"

#ifdef COMPONENT
#undef COMPONENT
#endif
#define COMPONENT "(AtmosphereFeed)"

namespace feed
{
parser::AtmosphereParser AtmosphereFeed::s_parser;

AtmosphereFeed::AtmosphereFeed(const std::string& name, const config::KeyValueMap& propertyMap,
                               std::shared_ptr<data::AtmosphereData> data)
    : Feed(name, COMPONENT, propertyMap, data)
{}

AtmosphereFeed::~AtmosphereFeed() noexcept
{}

Feed::Protocol AtmosphereFeed::get_protocol() const
{
    return Protocol::SENSOR;
}

bool AtmosphereFeed::process(const std::string& response)
{
    object::Atmosphere atmos(get_priority());
    if(s_parser.unpack(response, atmos))
    {
        logger.debug(m_component, "[", m_name, "] update: ", response);
        m_data->update(std::move(atmos));
    }
    return true;
}

}  // namespace feed

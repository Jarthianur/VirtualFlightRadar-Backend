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

#include "AprscFeed.h"

#include <stdexcept>

#include "../config/Configuration.h"
#include "../data/AircraftData.h"
#include "../object/Aircraft.h"
#include "../util/Logger.hpp"
#include "parser/AprsParser.h"

#ifdef COMPONENT
#    undef COMPONENT
#endif
#define COMPONENT "(AprscFeed)"

namespace feed
{
parser::AprsParser AprscFeed::s_parser;

AprscFeed::AprscFeed(const std::string& name, const config::KeyValueMap& propertyMap,
                     std::shared_ptr<data::AircraftData> data, std::int32_t maxHeight)
    : Feed(name, COMPONENT, propertyMap, data)
{
    parser::AprsParser::s_maxHeight = maxHeight;
    m_loginIt                       = m_propertyMap.find(KV_KEY_LOGIN);
    if (m_loginIt == m_propertyMap.end())
    {
        logger.warn(m_component, " could not find: ", m_name, "." KV_KEY_LOGIN);
        throw std::logic_error("No login given");
    }
}

AprscFeed::~AprscFeed() noexcept {}

Feed::Protocol AprscFeed::get_protocol() const
{
    return Protocol::APRS;
}

bool AprscFeed::process(const std::string& response)
{
    object::Aircraft ac(get_priority());
    if (s_parser.unpack(response, ac))
    {
        logger.debug(m_component, "[", m_name, "] update: ", response);
        m_data->update(std::move(ac));
    }
    return true;
}

const std::string& AprscFeed::get_login() const
{
    return m_loginIt->second;
}

}  // namespace feed

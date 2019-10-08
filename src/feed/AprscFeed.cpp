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
#include "feed/parser/AprsParser.h"
#include "object/Aircraft.h"
#include "util/Logger.hpp"

#ifdef COMPONENT
#    undef COMPONENT
#endif
#define COMPONENT "(AprscFeed)"

namespace feed
{
parser::AprsParser AprscFeed::s_parser;

AprscFeed::AprscFeed(const std::string& name, const config::Properties& properties,
                     std::shared_ptr<data::AircraftData> data, std::int32_t maxHeight)
    : Feed(name, COMPONENT, properties, data)
{
    parser::AprsParser::s_maxHeight = maxHeight;
    if (m_properties.get_property(KV_KEY_LOGIN, "-") == "-")
    {
        logger.warn(m_component, " could not find: ", m_name, "." KV_KEY_LOGIN);
        throw std::logic_error("No login given");
    }
}

Feed::Protocol AprscFeed::get_protocol() const
{
    return Protocol::APRS;
}

bool AprscFeed::process(const std::string& response)
{
    object::Aircraft ac(get_priority());
    if (s_parser.unpack(response, ac))
    {
        m_data->update(std::move(ac));
    }
    return true;
}

std::string AprscFeed::get_login() const
{
    return m_properties.get_property(KV_KEY_LOGIN);
}

}  // namespace feed

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

using namespace config;

namespace feed
{
parser::AprsParser AprscFeed::s_parser;

AprscFeed::AprscFeed(const std::string& name, const Properties& properties,
                     std::shared_ptr<data::AircraftData> data, std::int32_t maxHeight)
    : Feed(name, LOG_PREFIX, properties, data)
{
    parser::AprsParser::s_maxHeight = maxHeight;
    if (properties.get_property(Configuration::KV_KEY_LOGIN, "-") == "-")
    {
        logger.warn(m_logPrefix, "could not find: ", name, ".", Configuration::KV_KEY_LOGIN);
        throw std::logic_error("No login given");
    }
}

Feed::Protocol AprscFeed::getProtocol() const
{
    return Protocol::APRS;
}

bool AprscFeed::process(const std::string& response)
{
    try
    {
        m_data->update(s_parser.unpack(response, m_priority));
    }
    catch (const parser::UnpackError&)
    {}
    return true;
}

std::string AprscFeed::get_login() const
{
    return properties.get_property(Configuration::KV_KEY_LOGIN);
}

}  // namespace feed

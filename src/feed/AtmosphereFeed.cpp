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

#include "feed/AtmosphereFeed.h"

#include "config/Configuration.h"
#include "data/AtmosphereData.h"
#include "feed/parser/AtmosphereParser.h"
#include "object/Atmosphere.h"

using namespace config;

namespace feed
{
parser::AtmosphereParser AtmosphereFeed::s_parser;

AtmosphereFeed::AtmosphereFeed(str const& name, Properties const& properties,
                               s_ptr<data::AtmosphereData> data)
    : Feed(name, properties, data)
{}

Feed::Protocol AtmosphereFeed::protocol() const
{
    return Protocol::SENSOR;
}

bool AtmosphereFeed::process(str const& response)
{
    try
    {
        m_data->update(s_parser.unpack(response, m_priority));
    }
    catch (parser::UnpackError const&)
    {}
    return true;
}
}  // namespace feed

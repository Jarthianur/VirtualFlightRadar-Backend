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

#include "feed/FlarmFeed.h"

#include <unordered_map>

#include "config/Configuration.h"
#include "data/AircraftData.h"
#include "data/GpsData.h"
#include "feed/parser/FlarmParser.h"
#include "object/Aircraft.h"

#ifdef COMPONENT
#    undef COMPONENT
#endif
#define COMPONENT "(FlarmFeed)"

namespace feed
{
parser::FlarmParser FlarmFeed::s_parser;

FlarmFeed::FlarmFeed(const std::string& name, const config::Properties& properties,
                     std::shared_ptr<data::AircraftData> data, std::int32_t maxHeight,
                     std::shared_ptr<data::GpsData> gps)
    : Feed(name, COMPONENT, properties, data), m_gps(gps)
{
    parser::FlarmParser::s_maxHeight = maxHeight;
}

Feed::Protocol FlarmFeed::get_protocol() const
{
    return Protocol::FLARM;
}

bool FlarmFeed::process(const std::string& response)
{
    object::Aircraft ac(get_priority());
    ac.set_position(m_gps->get_position());
    if (s_parser.unpack(response, ac))
    {
        m_data->update(std::move(ac));
    }
    return true;
}

}  // namespace feed

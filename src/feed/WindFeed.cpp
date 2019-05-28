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

#include "feed/WindFeed.h"

#include <unordered_map>

#include "config/Configuration.h"
#include "data/WindData.h"
#include "feed/parser/WindParser.h"
#include "object/Wind.h"

#ifdef COMPONENT
#    undef COMPONENT
#endif
#define COMPONENT "(WindFeed)"

using namespace config;

namespace feed
{
parser::WindParser WindFeed::s_parser;

WindFeed::WindFeed(const std::string& name, const Properties& properties,
                   std::shared_ptr<data::WindData> data)
    : Feed(name, COMPONENT, properties, data)
{}

Feed::Protocol WindFeed::get_protocol() const
{
    return Protocol::SENSOR;
}

bool WindFeed::process(const std::string& response)
{
    object::Wind wind(get_priority());
    if (s_parser.unpack(response, wind))
    {
        m_data->update(std::move(wind));
    }
    return true;
}

}  // namespace feed

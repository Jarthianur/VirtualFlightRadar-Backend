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

#include "GpsFeed.h"

#include <stdexcept>
#include <unordered_map>

#include "../util/Logger.hpp"
#include "../config/Configuration.h"
#include "../data/GpsData.h"
#include "../object/GpsPosition.h"
#include "parser/GpsParser.h"

#ifdef COMPONENT
#undef COMPONENT
#endif
#define COMPONENT "(GpsFeed)"

namespace feed
{
parser::GpsParser GpsFeed::s_parser;

GpsFeed::GpsFeed(const std::string& name, const config::KeyValueMap& propertyMap,
                 std::shared_ptr<data::GpsData> data)
    : Feed(name, propertyMap, data)
{}

GpsFeed::~GpsFeed() noexcept
{}

Feed::Protocol GpsFeed::get_protocol() const
{
    return Protocol::GPS;
}

bool GpsFeed::process(const std::string& response)
{
    object::GpsPosition pos(get_priority());
    if(s_parser.unpack(response, pos))
    {
        try
        {
            if(m_data->update(std::move(pos)))
            {
                throw std::runtime_error("received good position -> stop");
            }
        }
        catch(const std::runtime_error& e)
        {
            logger.info(COMPONENT " ", m_name, ": ", e.what());
            return false;
        }
    }
    return true;
}

}  // namespace feed

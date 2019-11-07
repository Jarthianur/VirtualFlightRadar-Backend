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

#include "feed/GpsFeed.h"

#include "config/Configuration.h"
#include "data/GpsData.h"
#include "feed/parser/GpsParser.h"
#include "object/GpsPosition.h"
#include "util/Logger.hpp"

using namespace vfrb::config;

namespace vfrb::feed
{
constexpr auto     LOG_PREFIX = "(GpsFeed) ";
static auto const& logger     = Logger::instance();

GpsFeed::GpsFeed(str const& name, Properties const& properties, s_ptr<data::GpsData> data)
    : Feed(name, properties, data)
{}

Feed::Protocol GpsFeed::protocol() const
{
    return Protocol::GPS;
}

bool GpsFeed::process(str response)
{
    try
    {
        m_data->update(m_parser.unpack(std::move(response), m_priority));
    }
    catch ([[maybe_unused]] parser::error::UnpackError const&)
    {}
    catch (data::error::GpsDataException const& e)
    {
        logger.info(LOG_PREFIX, m_name, ": ", e.what());
        return false;
    }
    return true;
}
}  // namespace vfrb::feed

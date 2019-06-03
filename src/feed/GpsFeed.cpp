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

using namespace config;

namespace feed
{
parser::GpsParser GpsFeed::s_parser;

GpsFeed::GpsFeed(const std::string& name, const Properties& properties, std::shared_ptr<data::GpsData> data)
    : Feed(name, LOG_PREFIX, properties, data)
{}

Feed::Protocol GpsFeed::getProtocol() const
{
    return Protocol::GPS;
}

bool GpsFeed::process(const std::string& response)
{
    try
    {
        m_data->update(s_parser.unpack(response, m_priority));
    }
    catch (const parser::UnpackError&)
    {}
    catch (const data::GpsDataException& e)
    {
        logger.info(m_logPrefix, name, ": ", e.what());
        return false;
    }
    return true;
}

}  // namespace feed

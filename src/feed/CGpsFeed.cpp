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

#include "feed/CGpsFeed.hpp"

#include "config/CConfiguration.hpp"
#include "data/CGpsData.hpp"
#include "feed/parser/CGpsParser.hpp"
#include "object/CGpsPosition.hpp"

#include "CLogger.hpp"

using namespace vfrb::config;

namespace vfrb::feed
{
CTCONST            LOG_PREFIX = "(GpsFeed) ";
static auto const& logger     = CLogger::Instance();

CGpsFeed::CGpsFeed(String const& name_, CProperties const& prop_, SPtr<data::CGpsData> data_)
    : IFeed(name_, prop_, data_) {}

IFeed::EProtocol CGpsFeed::Protocol() const {
    return EProtocol::GPS;
}

bool CGpsFeed::Process(String str_) {
    try {
        m_data->Update(m_parser.Parse(std::move(str_), m_priority));
    } catch ([[maybe_unused]] parser::error::CParseError const&) {
    } catch (data::error::IGpsDataException const& e) {
        logger.Info(LOG_PREFIX, m_name, ": ", e.Message());
        return false;
    }
    return true;
}
}  // namespace vfrb::feed

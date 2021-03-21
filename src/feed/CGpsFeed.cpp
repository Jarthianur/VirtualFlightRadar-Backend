/*
    Copyright (C) 2016 Jarthianur
    A detailed list of copyright holders can be found in the file "docs/AUTHORS.md".

    This file is part of VirtualFlightRadar-Backend.

    VirtualFlightRadar-Backend is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VirtualFlightRadar-Backend is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VirtualFlightRadar-Backend.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "feed/CGpsFeed.hpp"

#include "config/CConfiguration.hpp"
#include "config/CProperties.hpp"
#include "data/CGpsData.hpp"
#include "feed/parser/CGpsParser.hpp"
#include "object/CGpsPosition.hpp"

#include "CLogger.hpp"

using vfrb::config::CProperties;

namespace vfrb::feed
{
CTCONST            LOG_PREFIX = "(GpsFeed) ";
static auto const& logger     = CLogger::Instance();

CGpsFeed::CGpsFeed(String const& name_, CProperties const& prop_, Shared<data::CGpsData> data_)
    : IFeed(name_, prop_, data_) {}

auto
CGpsFeed::Protocol() const -> IFeed::EProtocol {
    return EProtocol::GPS;
}

auto
CGpsFeed::Process(String&& str_) -> bool {
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

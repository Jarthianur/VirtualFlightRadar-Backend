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

#pragma once

#include "parser/CGpsParser.hpp"

#include "IFeed.hpp"

namespace vfrb::data
{
class CGpsData;
}  // namespace vfrb::data

namespace vfrb::feed
{
/**
 * @brief Extend Feed for GPS input.
 */
class CGpsFeed : public IFeed
{
    parser::CGpsParser const m_parser;  ///< Parser to unpack response from Client

public:
    /**
     * @param name       The unique name
     * @param properties The Properties
     * @param data       The GpsData container
     * @throw std::logic_error from parent constructor
     */
    CGpsFeed(String const& name_, config::CProperties const& prop_, SPtr<data::CGpsData> data_);

    /**
     * @brief Get this feeds Protocol.
     * @return Protocol::GPS
     */
    [[nodiscard]] auto
    Protocol() const -> EProtocol override;

    /**
     * @brief Implement Feed::process.
     */
    auto
    Process(String&& str_) -> bool override;
};
}  // namespace vfrb::feed

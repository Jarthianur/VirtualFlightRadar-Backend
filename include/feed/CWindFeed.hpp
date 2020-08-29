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

#include "parser/CWindParser.hpp"

#include "IFeed.hpp"

namespace vfrb::data
{
class CWindData;
}  // namespace vfrb::data

namespace vfrb::feed
{
/**
 * @brief Extend Feed for windsensor input.
 */
class CWindFeed : public IFeed
{
    parser::CWindParser const m_parser;  ///< Parser to unpack response from Client

public:
    /**
     * @param name       The SensorFeeds unique name
     * @param properties The Properties
     * @param data       The WindData contianer
     * @throw std::logic_error from parent constructor
     */
    CWindFeed(String const& name_, config::CProperties const& prop_, SPtr<data::CWindData> data_);

    /**
     * @brief Get this feeds Protocol.
     * @return Protocol::SENSOR
     */
    [[nodiscard]] auto
    Protocol() const -> EProtocol override;

    /**
     * @brief Feed::process.
     */
    auto
    Process(String str_) -> bool override;
};
}  // namespace vfrb::feed

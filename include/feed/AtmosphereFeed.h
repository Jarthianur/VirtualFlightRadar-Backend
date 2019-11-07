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

#pragma once

#include "parser/AtmosphereParser.h"

#include "Feed.h"

namespace vfrb::data
{
class AtmosphereData;
}  // namespace vfrb::data

namespace vfrb::feed
{
/**
 * @brief Extend Feed for sensor input.
 */
class AtmosphereFeed : public Feed
{
    NOT_COPYABLE(AtmosphereFeed)

    parser::AtmosphereParser const m_parser;  ///< Parser to unpack response from Client

public:
    /**
     * @param name       The unique name
     * @param properties The Properties
     * @param data       The WindData container
     * @throw std::logic_error from parent constructor
     */
    AtmosphereFeed(str const& m_name, config::Properties const& m_properties,
                   s_ptr<data::AtmosphereData> data);
    ~AtmosphereFeed() noexcept override = default;

    /**
     * @brief Get this feeds Protocol.
     * @return Protocol::SENSOR
     */
    Protocol protocol() const override;

    /**
     * @brief Implement Feed::process.
     */
    bool process(str response) override;
};
}  // namespace vfrb::feed

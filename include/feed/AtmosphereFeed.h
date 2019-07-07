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

#include <memory>
#include <string>

#include "config/Properties.h"
#include "util/defines.h"

#include "Feed.h"

namespace feed
{
namespace parser
{
class AtmosphereParser;
}  // namespace parser
}  // namespace feed

namespace data
{
class AtmosphereData;
}  // namespace data

namespace feed
{
/**
 * @brief Extend Feed for sensor input.
 */
class AtmosphereFeed : public Feed
{
    static constexpr auto LOG_PREFIX = "(AtmosphereFeed) ";

    static parser::AtmosphereParser s_parser;  ///< Parser to unpack response from Client

public:
    NOT_COPYABLE(AtmosphereFeed)

    /**
     * @param name       The unique name
     * @param properties The Properties
     * @param data       The WindData container
     * @throw std::logic_error from parent constructor
     */
    AtmosphereFeed(const std::string& name, const config::Properties& properties,
                   std::shared_ptr<data::AtmosphereData> data);
    ~AtmosphereFeed() noexcept override = default;

    /**
     * @brief Get this feeds Protocol.
     * @return Protocol::SENSOR
     */
    Protocol getProtocol() const override;

    /**
     * @brief Implement Feed::process.
     */
    bool process(const std::string& response) override;
};

}  // namespace feed

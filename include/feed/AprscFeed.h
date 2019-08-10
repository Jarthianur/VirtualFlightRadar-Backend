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

#include <cstdint>
#include <memory>
#include <string>

#include "config/Properties.h"
#include "util/WorkerThread.hpp"
#include "util/defines.h"

#include "Feed.h"

namespace feed
{
namespace parser
{
class AprsParser;
}  // namespace parser
}  // namespace feed

namespace data
{
class AircraftData;
}  // namespace data

namespace feed
{
/**
 * @brief Extend Feed for APRSC protocol.
 */
class AprscFeed : public Feed
{
    static constexpr auto LOG_PREFIX = "(AprscFeed) ";

    static parser::AprsParser       s_parser;  ///< Parser to unpack response from Client
    util::WorkerThread<std::string> m_worker;

public:
    NOT_COPYABLE(AprscFeed)

    /**
     * @param name       The unique name
     * @param properties The Properties
     * @param data       The AircraftData container
     * @param maxHeight  The max height filter
     * @throw std::logic_error if login is not given, or from parent constructor
     */
    AprscFeed(const std::string& name, const config::Properties& propertyMap,
              std::shared_ptr<data::AircraftData> data, std::int32_t maxHeight);
    ~AprscFeed() noexcept override = default;

    /**
     * @brief Get this feeds Protocol.
     * @return Protocol::APRS
     */
    Protocol protocol() const override;

    /**
     * @brief Implement Feed::process.
     */
    bool process(const std::string& response) override;

    /**
     * @brief Get the login string.
     * @return the login
     */
    std::string login() const;
};

}  // namespace feed

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
#include <unordered_map>

#include "../config/Properties.h"
#include "../Defines.h"
#include "Feed.h"

namespace feed
{
namespace parser
{
class AprsParser;
} /* namespace parser */
} /* namespace feed */

/// @namespace data
namespace data
{
class AircraftData;
}  // namespace data

/// @namespace feed
namespace feed
{
/**
 * @class AprscFeed
 * @brief APRSC input feed.
 * @extends Feed
 */
class AprscFeed : public Feed
{
public:
    NOT_COPYABLE(AprscFeed)

    /**
     * @fn AprscFeed
     * @brief Constructor
     * @param crName     The unique name
     * @param crKvMap    The properties map
     * @param pData      The AircraftData pointer
     * @param vMaxHeight The max height filter
     * @throw std::logic_error if login is not given or from parent constructor
     */
    AprscFeed(const std::string& name, const config::KeyValueMap& propertyMap,
              std::shared_ptr<data::AircraftData> data, std::int32_t maxHeight);

    /**
     * @fn ~AprscFeed
     * @brief Destructor
     */
    ~AprscFeed() noexcept;

    Protocol get_protocol() const override;

    /**
     * @see Feed#process
     */
    bool process(const std::string& response) noexcept override;

    const std::string& get_login() const;

private:
    /// @var mParser
    /// Parser to unpack response from Client
    static parser::AprsParser s_parser;

    config::KeyValueMap::const_iterator m_loginIt;
};

}  // namespace feed

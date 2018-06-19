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

#include "../Defines.h"
#include "../config/PropertyMap.h"

namespace data
{
class Data;
} /* namespace data */
namespace feed
{
namespace client
{
class Client;
class ClientManager;
} /* namespace client */
} /* namespace feed */

/// @namespace feed
namespace feed
{
/**
 * @class Feed
 * @brief Base class representing an input feed.
 */
class Feed
{
public:
    NON_COPYABLE(Feed)

    /**
     * @fn ~Feed
     * @brief Destructor
     */
    virtual ~Feed() noexcept;

    virtual void registerToClient(client::ClientManager& rManager) = 0;

    /**
     * @fn process
     * @brief Handle Client's response.
     * @param crResponse The response
     */
    virtual void process(const std::string& crResponse) noexcept = 0;

    /**
     * Define and declare getters.
     */
    GETTER_CR(std::string, mName, Name)
    GETTER_V(std::uint32_t, mPriority, Priority)

protected:
    /**
     * @fn Feed
     * @brief Constructor
     * @param crName  The Feeds unique name
     * @param crKvMap The properties map
     * @throw std::logic_error if host or port are not given
     */
    Feed(const std::string& crName, const config::KeyValueMap& crKvMap,
         std::shared_ptr<data::Data> pData);

    /// @var mName
    /// Unique name
    const std::string mName;

    /// @var mKvMap
    /// Key-value-map holding the properties.
    const config::KeyValueMap mKvMap;

    std::weak_ptr<client::Client> mSubsribedClient;

    std::shared_ptr<data::Data> mpData;

private:
    /**
     * @fn initPriority
     * @brief Initialize the priority from the given properties.
     */
    void initPriority() noexcept;

    /// @var mPriority
    /// The priority
    std::uint32_t mPriority;
};

}  // namespace feed

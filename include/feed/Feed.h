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

#include "client/net/Endpoint.hpp"
#include "config/Properties.h"
#include "util/defines.h"

namespace data
{
class Data;
}  // namespace data

namespace feed
{
/**
 * @brief Base class for input feeds.
 */
class Feed
{
    //< begin methods >//
    /**
     * @brief Initialize the priority from the given properties.
     */
    std::uint32_t initPriority() const noexcept;
    //< end methods >//

protected:
    //< begin members >//
    std::shared_ptr<data::Data> m_data;       ///< Respective Data container
    const std::uint32_t         m_priority;   ///< Priority
    const char* const           m_logPrefix;  ///< Component string
    //< end members >//

    /**
     * @param name       The Feeds unique name
     * @param component  The component string
     * @param properties The Properties
     * @throw std::logic_error if host or port are not given in properties
     */
    Feed(const std::string& name, const char* logPrefix, const config::Properties& propertyMap,
         std::shared_ptr<data::Data> data);

public:
    /**
     * @brief The protocol that the Feed supports.
     */
    enum class Protocol : std::uint_fast8_t
    {
        APRS,
        SBS,
        GPS,
        SENSOR
    };

    //< begin members >//
    const std::string        name;        ///< Unique name
    const config::Properties properties;  ///< Properties
    //< end members >//

    NOT_COPYABLE(Feed)
    virtual ~Feed() noexcept = default;

    //< begin interfaces >//
    /**
     * @brief Get the supported Protocol.
     * @return the protocol
     */
    virtual Protocol getProtocol() const = 0;

    /**
     * @brief Get the feeds required Endpoint.
     * @return the endpoint
     */
    client::net::Endpoint getEndpoint() const;

    /**
     * @brief Handle client's response.
     * @param response The response
     */
    virtual bool process(const std::string& response) = 0;
    //< end interfaces >//
};

}  // namespace feed
